// Copyright Epic Games, Inc. All Rights Reserved.

#include "BMPCharacter.h"
#include "BMPProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/InteractableInterface.h"
#include "AbilitySystemComponent.h"
#include "BMPAttributeSetBase.h"
#include "BMPWeapon.h"

//////////////////////////////////////////////////////////////////////////
// ABMPCharacter

ABMPCharacter::ABMPCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSetBase = CreateDefaultSubobject<UBMPAttributeSetBase>("AttributeSetBase"); /*There is a known bug (not on my end) with child blueprints and attribute sets*/
	AttributeSetBase->InitHealth(100.F);
	AttributeSetBase->InitMaxHealth(100.F);
	AbilitySystemComponent->AddSpawnedAttribute(AttributeSetBase);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABMPCharacter::HandleHealthChanged);

}

void ABMPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABMPCharacter, Health)
	DOREPLIFETIME(ABMPCharacter, Weapon)
}

void ABMPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////// Input

void ABMPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABMPCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABMPCharacter::Look);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ABMPCharacter::Interact);
	}
}

void ABMPCharacter::OnRep_Health()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s's Current Health is %f."), *GetNameSafe(this), Health)
	}

	if(IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Your Current Health is %f."), Health)
	}
}

void ABMPCharacter::OnRep_Weapon()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponChanged"))
	if (IsLocallyControlled() && Weapon != nullptr) //if locally controlled, if weapon isnt nullptr
	{
		/*
		We cant call OnRep_Weapon() which needs to be called if a listen-server wants to equip the weapon.
		We would have to change the replicated value within the character. This is for good organization.
		*/

		UE_LOG(LogTemp, Warning, TEXT("OnRep_Weapon::IsLocallyControlled()"))
		//We need to ask to bind inputs and pass in our input component or something.
	}
}

float ABMPCharacter::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

void ABMPCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Display, TEXT("%s: HandleHealthChanged"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));
}

void ABMPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABMPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABMPCharacter::Interact()
{
	float MinDistanceSquared = TNumericLimits<float>::Max();
	IInteractableInterface* NearestInteractable = nullptr;
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappedActor : OverlappingActors)
	{
		if (IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(OverlappedActor))
		{
			float DistanceSquared = FVector::DistSquared(GetActorLocation(), OverlappedActor->GetActorLocation());

			if (DistanceSquared < MinDistanceSquared)
			{
				MinDistanceSquared = DistanceSquared;
				NearestInteractable = InteractableActor;
			}
		}
	}
	
	if (NearestInteractable != nullptr)
	{
		NearestInteractable->Interact(this);
	}
}

void ABMPCharacter::EquipWeapon(ABMPWeapon* NewWeapon)
{
	if (NewWeapon)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			Weapon = NewWeapon;
			Weapon->OnEquip(this);
		}
		else
		{
			ServerEquipWeapon(NewWeapon);
		}
	}
}

void ABMPCharacter::ServerEquipWeapon_Implementation(ABMPWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}