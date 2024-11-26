
#include "BMPWeapon.h"
#include "BMP/BMPPickupComponent.h"
#include "BMP/BMPCharacter.h"
#include "BMPProjectile.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Weapon/BMPWeaponState.h"
#include "Weapon/BMPWeaponStateIdle.h"
#include "Weapon/BMPWeaponStateFiring.h"
#include "Weapon/BMPWeaponStateReloading.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "AbilitySystemComponent.h"
#include"AbilitySystemInterface.h"

#include "Components/DecalComponent.h"


// Sets default values
ABMPWeapon::ABMPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	SetRootComponent(Mesh3P);
	Mesh3P->bOwnerNoSee = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetupAttachment(GetRootComponent());
	Mesh1P->bOnlyOwnerSee = true;

	PickUpComponent = CreateDefaultSubobject<UBMPPickupComponent>("PickUpComponent");
	PickUpComponent->SetupAttachment(GetRootComponent());

	HitscanRange = 15000.f;

	bWantsToFire = false;
	bIsFiring = false;

	bReplicates = true;

	CurrentState = nullptr;
	IdleState = CreateDefaultSubobject<UBMPWeaponStateIdle>("IdleState");
	FiringState = CreateDefaultSubobject<UBMPWeaponStateFiring>("FiringState");
	ReloadingState = CreateDefaultSubobject<UBMPWeaponStateReloading>("ReloadingState");

	MaxAmmoReserves = 120;
	CurrentAmmoReserves = MaxAmmoReserves;
	MagazineSize = 39;
	CurrentAmmo = MagazineSize;
	AmmoCost = 1;
	FireRateSeconds = 0.2f;
	ReloadTimeSeconds = 0.33f;
	LastTimeFiredSeconds = -1.f;
}

void ABMPWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABMPWeapon, Character);

	DOREPLIFETIME_CONDITION(ABMPWeapon, MaxAmmoReserves, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABMPWeapon, CurrentAmmoReserves, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABMPWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABMPWeapon, MagazineSize, COND_OwnerOnly);
}

void ABMPWeapon::Interact(ABMPCharacter* TargetCharacter)
{
	if (TargetCharacter)
	{
		TargetCharacter->EquipWeapon(this);
	}
}

void ABMPWeapon::OnInteractionRangeEntered(ABMPCharacter* TargetCharacter)
{
}

void ABMPWeapon::OnInteractionRangeExited(ABMPCharacter* TargetCharacter)
{
}

// Called when the game starts or when spawned
void ABMPWeapon::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = IdleState;
}

// Called every frame
void ABMPWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateRecoil(DeltaSeconds);
}

void ABMPWeapon::UpdateRecoil(float DeltaSeconds)
{
	float VerticalRecoilToApplyThisFrame = FMath::Clamp(RecoilOffset.Z, 0.f, DeltaSeconds * RecoilSpeed);
	RecoilOffset.Z -= VerticalRecoilToApplyThisFrame; 	if (Character)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		
		if(PlayerController)
		{
			PlayerController->AddPitchInput(-VerticalRecoilToApplyThisFrame);
		}
	}
	if (!bIsFiring)
	{
	}
}

void ABMPWeapon::OnEquip(ABMPCharacter* NewCharacter) //want to know as little about the character as possible. If the character dies, we need to ensure the input works properly for each person picking it up, not anyone else.
{
	Character = NewCharacter;
	OnRep_Character();
	if (Character == nullptr)
	{
		return;
	}
	SetOwner(Character);
	SetInstigator(Character);
	//Move this code to another function possibly. Keep character == nullptr check

	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	if (GetMesh3P())
	{
		GetMesh3P()->AttachToComponent(Character->GetMesh(), TransformRules, "GripPoint");
	}
	if (GetMesh1P())
	{
		GetMesh1P()->AttachToComponent(Character->GetMesh1P(), TransformRules, "GripPoint");
	}
}

void ABMPWeapon::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
	}
	
	bWantsToFire = true;
	CurrentState->HandleFireInput();
}

void ABMPWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

void ABMPWeapon::StopFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopFire();
	}

	bWantsToFire = false;
}

void ABMPWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

void ABMPWeapon::Fire()
{
	UE_LOG(LogTemp, Display, TEXT("Running on %s"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));
	if (GetNetMode() != NM_DedicatedServer)
	{
		PlayFiringEffects();
	}

	if (Character && Character->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Display, TEXT("Locally controlled by: %s"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));

		if (ProjectileClass)
		{
			FireProjectile();
		}
		else
		{
			FireHitscan();
		}
	}

	ApplyRecoil();

	AddAmmo(-AmmoCost);

	LastTimeFiredSeconds = GetWorld()->GetTimeSeconds();
}

void ABMPWeapon::PlayFiringEffects()
{
	UGameplayStatics::PlaySoundAtLocation(this, FireSoundCue, GetActorLocation(), 0.35, 1.F, 0.0f);
}

void ABMPWeapon::FireHitscan()
{
	if (Character == nullptr)
	{
		return;
	}
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	FHitResult HitResult;
	FVector AimLocation;
	FRotator AimRotation;
	
	if (PlayerController)
	{
		PlayerController->GetPlayerViewPoint(AimLocation, AimRotation);
		FVector StartTrace = AimLocation;
		FVector EndTrace = StartTrace + (AimRotation.Vector() * HitscanRange);
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Character);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);
		if (bHit)
		{
			ServerProcessHit(HitResult);
		}
	}
}

void ABMPWeapon::ServerProcessHit_Implementation(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(HitActor);
	if (!AbilitySystemInterface)
	{
		return;
	}
	UAbilitySystemComponent* TargetAbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!TargetAbilitySystemComponent)
	{
		return;
	}
	check(GetInstigator())
	if (Character->GetAbilitySystemComponent())
	{
		if (DamageEffect)
		{
			FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddHitResult(HitResult);

			FPredictionKey PredictionKey;
			const FGameplayEffectSpecHandle DamageEffectSpec = TargetAbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 0.F, EffectContext);

			Character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*DamageEffectSpec.Data, TargetAbilitySystemComponent);
		}
	}
}

void ABMPWeapon::FireProjectile()
{
	if (Character == nullptr)
	{
		return;
	}
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	FVector AimLocation;
	FRotator AimRotation;

	if (PlayerController)
	{
		PlayerController->GetPlayerViewPoint(AimLocation, AimRotation);
		ServerFireProjectile(AimLocation, AimRotation);
	}
}

void ABMPWeapon::OnRep_Character()
{
	BindInput(Character); 
}

void ABMPWeapon::BindInput(ABMPCharacter* TargetCharacter)
{
	if (TargetCharacter == nullptr)
	{
		return;
	}
	APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABMPWeapon::StartFire);
			EnhancedInputComponent->BindAction(StopFireAction, ETriggerEvent::Completed, this, &ABMPWeapon::StopFire);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABMPWeapon::HandleReloadInput);
		}
	}
}

void ABMPWeapon::GotoState(UBMPWeaponState* NewState)
{
	UBMPWeaponState* PreviousState = CurrentState;
	CurrentState = NewState;

	if (PreviousState != nullptr)
	{
		PreviousState->ExitState();
	}
	if (CurrentState != nullptr)
	{
		CurrentState->EnterState();
	}
}

void ABMPWeapon::GotoStateIdle()
{
	GotoState(IdleState);
}

void ABMPWeapon::GotoStateReloading()
{
	GotoState(ReloadingState);
}

void ABMPWeapon::GotoStateFiring()
{
	GotoState(FiringState);
}

bool ABMPWeapon::IsReadyToFire() const
{
	if ((GetWorld()->GetTimeSeconds() - LastTimeFiredSeconds) >= (FireRateSeconds))
	{
		return true;
	}
	return false;
}

void ABMPWeapon::HandleReloadInput()
{
	CurrentState->HandleReloadInput();
}

void ABMPWeapon::ReloadWeapon()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		int BulletDifference = (MagazineSize - CurrentAmmo);
		int AmountToReload = FMath::Min(BulletDifference, CurrentAmmoReserves);
		CurrentAmmo += AmountToReload;
		CurrentAmmoReserves -= AmountToReload;
	}
	else
	{
		ServerReloadWeapon();
	}
}

void ABMPWeapon::ApplyRecoil()
{
	RecoilOffset.Z += MaxVerticalRecoilOffsetPerShot;
	UE_LOG(LogTemp, Warning, TEXT("RecoilOFfset.Z: %f"), RecoilOffset.Z);
}

void ABMPWeapon::ServerReloadWeapon_Implementation()
{
	ReloadWeapon();
}

bool ABMPWeapon::CanReload() const
{
	return (CurrentAmmoReserves > 0 && CurrentAmmo < MagazineSize);
}

bool ABMPWeapon::CanFire() const
{
	return HasAmmoInMagazine() && IsReadyToFire();
}

void ABMPWeapon::AddAmmo(float Amount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MagazineSize);
}

void ABMPWeapon::AddAmmoReserves(float Amount)
{
	CurrentAmmoReserves = FMath::Clamp(CurrentAmmoReserves + Amount, 0, MaxAmmoReserves);
}

void ABMPWeapon::ServerFireHitscan_Implementation(FVector StartTrace, FVector EndTrace)
{
}

void ABMPWeapon::ServerFireProjectile_Implementation(FVector AimLocation, FRotator AimRotation)
{
	UE_LOG(LogTemp, Display, TEXT("Running on %s"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));
	if (ProjectileClass)
	{
		//owner could be this weapon, or the character
		FTransform AimTransform(AimRotation, AimLocation);
		ABMPProjectile* Projectile = Cast<ABMPProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, AimTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));

		//Might be unnecessary.
		Projectile->GetCollisionComp()->MoveIgnoreActors.Add(this);
		Projectile->GetCollisionComp()->MoveIgnoreActors.Add(Character);

		UGameplayStatics::FinishSpawningActor(Projectile, AimTransform);
	}
}