
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

#include "Sound/SoundCue.h"

// Sets default values
ABMPWeapon::ABMPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	SetRootComponent(Mesh3P);
	PickUpComponent = CreateDefaultSubobject<UBMPPickupComponent>("PickUpComponent");
	PickUpComponent->SetupAttachment(GetRootComponent());
	HitscanRange = 15000.f;

	bWantsToFire = false;

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
void ABMPWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetMesh3P()->AttachToComponent(Character->GetMesh(), TransformRules, "hand_r");

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
	if (GetNetMode() != NM_DedicatedServer)
	{
		PlayFiringEffects();
	}

	if (Character && Character->IsLocallyControlled())
	{
		if (ProjectileClass)
		{
			FireProjectile();
		}
		else
		{
			FireHitscan();
		}
	}

	AddAmmo(-AmmoCost);

	LastTimeFiredSeconds = GetWorld()->GetTimeSeconds();
}

void ABMPWeapon::PlayFiringEffects()
{
	//UE_LOG(LogTemp, Warning, TEXT("PlayingFiringEffects"))
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
		FCollisionQueryParams CollisionQueryParams;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
		if (bHit)
		{
			ServerProcessHit(HitResult);
		}
		//UE_LOG(LogTemp, Display, TEXT("%s: Hitscan"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));
		
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::White, false, 35.f);
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
	UE_LOG(LogTemp, Display, TEXT("%s: OnRepCharacter"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));

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

/*
Need start and stop methods for input. So a start  fire, server start fire etc. bWantsToFire really matters on authority.

Server determines state. we are just input. think about player feel even at high pings.
*/

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

void ABMPWeapon::ServerProcessHit_Implementation(const FHitResult& HitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerProcessHit"))
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
	FHitResult HitResult;
	//@todo: fix collision channels
	FCollisionQueryParams CollisionResponseParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CollisionResponseParams);
	if (bHit)
	{

	}
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 1.5f);

}

void ABMPWeapon::ServerFireProjectile_Implementation(FVector AimLocation, FRotator AimRotation)
{
	if (ProjectileClass)
	{
		//owner could be this weapon, or the character
		FTransform AimTransform(AimRotation, AimLocation);
		ABMPProjectile* Projectile = Cast<ABMPProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, AimTransform));

		//Might be unnecessary.
		Projectile->GetCollisionComp()->MoveIgnoreActors.Add(this);
		Projectile->GetCollisionComp()->MoveIgnoreActors.Add(Character);

		UGameplayStatics::FinishSpawningActor(Projectile, AimTransform);
	}
}





