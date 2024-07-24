
#include "BMPWeapon.h"
#include "BMP/BMPPickupComponent.h"
#include "BMP/BMPCharacter.h"
#include "BMPProjectile.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABMPWeapon::ABMPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	SetRootComponent(Mesh3P);
	PickUpComponent = CreateDefaultSubobject<UBMPPickupComponent>("PickUpComponent");
	PickUpComponent->SetupAttachment(GetRootComponent());
	HitscanRange = 1500.f;

	bWantsToFire = false;

	bReplicates = true;
}

void ABMPWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABMPWeapon, Character);

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

void ABMPWeapon::HandleFireInput()
{
	bWantsToFire = true;

	Fire();
	
}

void ABMPWeapon::Fire()
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
		UE_LOG(LogTemp, Display, TEXT("%s: Hitscan"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 1.5f);
		ServerFireHitscan(StartTrace, EndTrace);
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
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABMPWeapon::HandleFireInput);
		}
	}
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



