// Copyright Epic Games, Inc. All Rights Reserved.

#include "BMPCharacter.h"
#include "BMPCharacterMovementComponent.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interfaces/InteractableInterface.h"
#include "BMPWeapon.h"
#include "Game/BMPGameMode.h"

#include "Net/UnrealNetwork.h"

#include "AbilitySystemComponent.h"
#include "BMPAttributeSetBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

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
	Mesh1P->SetupAttachment(GetFirstPersonCameraComponent());
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSetBase = CreateDefaultSubobject<UBMPAttributeSetBase>("AttributeSetBase"); /*There is a known bug (not on my end) with child blueprints and attribute sets*/
	AttributeSetBase->InitHealth(100.F);
	AttributeSetBase->InitMaxHealth(100.F);

	AbilitySystemComponent->AddSpawnedAttribute(AttributeSetBase);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABMPCharacter::HandleHealthChanged);

	LastDamageTakenInfo.bIsDead = false;
	LastDamageTakenInfo.DamageInstigator = nullptr;

	BMPCharacterMovement = Cast<UBMPCharacterMovementComponent>(GetCharacterMovement());
}

void ABMPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABMPCharacter, Weapon)
	DOREPLIFETIME(ABMPCharacter, LastDamageTakenInfo)
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

		//Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABMPCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABMPCharacter::EndCrouch);

	}
}

void ABMPCharacter::OnRep_Weapon()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponChanged"))
	if (IsLocallyControlled() && Weapon != nullptr)
	{
		Weapon->OnEquip(this);
	}
}

float ABMPCharacter::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

void ABMPCharacter::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AActor* DamageInstigator = nullptr;
		if (Data.GEModData)
		{
			const FGameplayEffectSpec& EffectSpec = Data.GEModData->EffectSpec;
			const FGameplayEffectContextHandle& EffectContext = EffectSpec.GetContext();

			DamageInstigator = EffectContext.GetInstigator();
		}

		if(Data.NewValue <= 0.0f && !LastDamageTakenInfo.bIsDead)
		{
			Die(DamageInstigator);
		}
	}
}

void ABMPCharacter::Die(AActor* DeathInstigator)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (GetWorld())
		{
			APawn* DeathInstigatorPawn = Cast<APawn>(DeathInstigator);
			if (DeathInstigatorPawn)
			{
				GetWorld()->GetAuthGameMode<ABMPGameMode>()->NotifyPawnDeath(DeathInstigatorPawn, this);
			}
		}
	}

	if (Controller)
	{
		Controller->UnPossess();
	}

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	LastDamageTakenInfo.bIsDead = true;
	LastDamageTakenInfo.DamageInstigator = DeathInstigator;

	return;
}

void ABMPCharacter::OnRep_LastDamageTakenInfo()
{
	UE_LOG(LogTemp, Warning, TEXT(""))
	if (LastDamageTakenInfo.bIsDead)
	{
		Die(LastDamageTakenInfo.DamageInstigator);
	}
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

//very quick interaction system to test weapon functionality
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

void ABMPCharacter::StartCrouch(const FInputActionValue& Value)
{
	//There is crouching functionality within the character and cmc for different usages.
	//Telling a character to crouch should be easily done via reference of the character as well as querying it
	//Things like wanting to do an action arent relevant in a situation like that. Thats more internal to the CMC.
	if (GetBMPCharacterMovement())
	{
		if (CanSlide()) //Maybe I dont decide here, and decide inside the character movement component. This is exploitable
		{
			GetBMPCharacterMovement()->bWantsToSlide = true;
		}
		else
		{
			Crouch();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Custom movement Component"));
	}
}

void ABMPCharacter::EndCrouch(const FInputActionValue& Value)
{
	if (GetBMPCharacterMovement())
	{
		GetBMPCharacterMovement()->bWantsToSlide = false;
	}
	UnCrouch();
}

bool ABMPCharacter::CanSlide() const
{	//Forward Velocity Calculation  to see if speed passes threshold.
	return !bIsSliding && BMPCharacterMovement && FVector::DotProduct(GetVelocity(), GetActorForwardVector()) > BMPCharacterMovement->ForwardVelocityNeededToSlide; //add simulating physics check later (want to see without).
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