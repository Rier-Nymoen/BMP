


#include "BMPCharacterMovementComponent.h"
#include "BMPCharacter.h"

UBMPCharacterMovementComponent::UBMPCharacterMovementComponent()
{
	ForwardVelocityNeededToSlide = 732.f;

	bCanSlide = true;
	bWantsToSlide = false;
}

void UBMPCharacterMovementComponent::InitializeComponent()
{
	BMPCharacterOwner = Cast<ABMPCharacter>(GetOwner());
}
/*Probably need to incorporate logic similar to "can we crouch in this state?" logic.
* Just because wants to slide = true doesnt mean we can just let that happen.
* Whats the difference between the canslide check in the character vs in the character movement component?
*/

//Update the movement mode to custom sliding mode if all works out.

//How do the physics work?

/*And how do we restore states after. Theres several conditions and things that can happen.Some helper functions must make this job a lot easier
since*/
void UBMPCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	//need holding slide button off of landing to initiate slide
	//able to exploit speed requirement unintentionally with variable for being able to slide being set.

	const bool bIsSliding = IsSliding();
	if (!bIsSliding && bWantsToSlide && CanSlideInCurrentState()) //Eventually make a "can slide in current state". Based on crouching.
	{
		Slide();
	}
	else if(bIsSliding && (!bWantsToSlide || !CanSlideInCurrentState()))
	{
		EndSlide();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UBMPCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
}

void UBMPCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}

void UBMPCharacterMovementComponent::PhysSliding(float deltaTime, int32 Iterations)
{
}

bool UBMPCharacterMovementComponent::IsSliding() const
{
	return BMPCharacterOwner && BMPCharacterOwner->bIsSliding;
}


bool UBMPCharacterMovementComponent::CanSlideInCurrentState() const //Should I check the velocity here? Otherwise I fear I can exploit sliding at lower velocities without rechecking.
{
	if (bCanSlide == false)
	{
		return false;
	}
	return !IsFalling() || IsMovingOnGround(); //simulating physics stuff.
}

void UBMPCharacterMovementComponent::Slide() 
{
	if (!HasValidData()) //
	{
		UE_LOG(LogTemp, Warning, TEXT("Doesnt have valid data"))
		return;
	}
	if (!BMPCharacterOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("valid bmp character onwer"))

		return;
	}
	UE_LOG(LogTemp,Warning, TEXT("Slide"))
	BMPCharacterOwner->bIsSliding = true;
}

void UBMPCharacterMovementComponent::EndSlide()
{
	if (!HasValidData()) //
	{
		return;
	}
	if (!BMPCharacterOwner)
	{
		return;
	}
	BMPCharacterOwner->bIsSliding = false;
	UE_LOG(LogTemp, Warning, TEXT("EndingSlide"))

}

