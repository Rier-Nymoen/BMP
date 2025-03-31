

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BMPCharacterMovementComponent.generated.h"

class ABMPCharacter;

UENUM()
enum EBMPMovementMode : uint8
{
	BMPMove_None,
	BMPMove_Sliding
};
/**
 * 
 */
UCLASS()
class BMP_API UBMPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UBMPCharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void InitializeComponent() override;
	UPROPERTY()//todo understand transient
	ABMPCharacter* BMPCharacterOwner;

public:
	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite)
	bool bCanSlide;

	bool bWantsToSlide;
protected:
	
	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float ForwardVelocityNeededToSlide;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float VelocityNeededToSlide;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = ""))
	float SlideImpulse;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = ""))
	float SlideFriction;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float BrakingDecelerationSliding;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxSlidingSpeed;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideStrafeControl;

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float SlideGravityCoefficient;

public:

	ABMPCharacter* GetBMPCharacterOwner() const { return BMPCharacterOwner; }

	virtual float GetMaxBrakingDeceleration() const override;
	virtual float GetCustomMaxBrakingDeceleration() const;

	virtual float GetMaxSpeed() const override;
	virtual float GetCustomMaxSpeed() const;

	virtual bool IsSliding() const;

	float GetForwardVelocity() const;

	//Last Update if was falling.
	bool WasFalling();

	virtual bool IsMovingOnGround() const override;


protected:
	//virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	
	virtual void PerformMovement(float DeltaTime) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual void PhysSliding(float deltaTime, int32 Iterations);

	virtual bool HasEnoughVelocityToEnterSlide() const;

	virtual bool CanSlideInCurrentState() const;

	virtual void Slide();

	virtual void EndSlide();

	virtual bool CanCrouchInCurrentState() const override;
protected:

	void HelperDrawVectorFromPlayer(FVector Vector, float Length, FColor Color, bool bPersistent, float LifeTime = (-1.0f), FVector Offset = FVector::ZeroVector) const;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override; 

	EMovementMode PrevMovementMode;

	EBMPMovementMode PrevBMPMovementMode;
};
