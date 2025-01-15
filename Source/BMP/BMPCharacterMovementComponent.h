

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BMPCharacterMovementComponent.generated.h"

class ABMPCharacter;

UENUM()
enum EBMPMovementMode : int
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
protected:
	virtual void InitializeComponent() override;
	UPROPERTY()//todo understand transient
	ABMPCharacter* BMPCharacterOwner;
public:
	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite)
	bool bCanSlide;

	bool bWantsToSlide;
	

	UPROPERTY(Category = "Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float ForwardVelocityNeededToSlide;

	ABMPCharacter* GetBMPCharacterOwner() const { return BMPCharacterOwner; }
protected:
	//virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	
	virtual void PerformMovement(float DeltaTime) override;

	virtual void PhysSliding(float deltaTime, int32 Iterations);

	virtual bool IsSliding() const;

	virtual bool CanSlideInCurrentState() const;

	virtual void Slide();

	virtual void EndSlide();



};
