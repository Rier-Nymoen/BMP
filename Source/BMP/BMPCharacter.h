// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "BMPCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

class ABMPWeapon;
class UAbilitySystemComponent;

struct FOnAttributeChangeData;

USTRUCT()
struct FDamageTakenInfo
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsDead;

	UPROPERTY()
	AActor* DamageInstigator;
};

UCLASS(config=Game)
class ABMPCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ABMPCharacter();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay();

public:
		
	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	void EquipWeapon(class ABMPWeapon* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(class ABMPWeapon* NewWeapon);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Interact();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

		/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TestFunctionAction;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(ReplicatedUsing = OnRep_Weapon)
	ABMPWeapon* Weapon;

	ABMPWeapon* GetEquippedWeapon() const { return Weapon; }

protected:

	UFUNCTION()
	void OnRep_Weapon();
	
public:
	float GetHealth() const;

	UAbilitySystemComponent* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	class UBMPAttributeSetBase* AttributeSetBase;

protected:
	virtual void HandleHealthChanged(const FOnAttributeChangeData& Data);

	virtual void Die(AActor* DeathInstigator);

	UPROPERTY(ReplicatedUsing = OnRep_LastDamageTakenInfo)
	FDamageTakenInfo LastDamageTakenInfo;

	UFUNCTION()
	void OnRep_LastDamageTakenInfo();

};

