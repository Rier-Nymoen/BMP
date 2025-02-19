


#include "BMPCharacterMovementComponent.h"
#include "BMPCharacter.h"
#include "Components/CapsuleComponent.h"

UBMPCharacterMovementComponent::UBMPCharacterMovementComponent()
{
	ForwardVelocityNeededToSlide = 732.f;
	VelocityNeededToSlide = 100.f;
	bCanSlide = true;
	bWantsToSlide = false;
}

void UBMPCharacterMovementComponent::InitializeComponent()
{
	BMPCharacterOwner = Cast<ABMPCharacter>(GetOwner());
}
float UBMPCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return BrakingDecelerationWalking;
	case MOVE_Falling:
		return BrakingDecelerationFalling;
	case MOVE_Swimming:
		return BrakingDecelerationSwimming;
	case MOVE_Flying:
		return BrakingDecelerationFlying;
	case MOVE_Custom:
		return GetCustomMaxBrakingDeceleration();
	case MOVE_None:
	default:
		return 0.f;
	}
}
float UBMPCharacterMovementComponent::GetCustomMaxBrakingDeceleration() const
{
	switch (CustomMovementMode)
	{	
	case BMPMove_Sliding:
		return BrakingDecelerationSliding;
	default:
		return 0.0f;
	}
}
void UBMPCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	const bool bIsSliding = IsSliding();
	if (!bIsSliding && bWantsToCrouch && CanSlideInCurrentState() && HasEnoughVelocityToEnterSlide()) //Eventually make a "can slide in current state". Based on crouching.
	{
		Slide();
	}
	else if (bIsSliding && (!bWantsToCrouch || !CanSlideInCurrentState()))
	{
		EndSlide();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

//probably misunderstanding how to use this function.
void UBMPCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	if (IsSliding() && !CanSlideInCurrentState())
	{
		EndSlide();
	}
}

void UBMPCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}

void UBMPCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case EBMPMovementMode::BMPMove_None:
		break;
	case EBMPMovementMode::BMPMove_Sliding:
		PhysSliding(deltaTime, Iterations);
		break;
	default:
		break;
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UBMPCharacterMovementComponent::PhysSliding(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	//These vectors are incorrect for some reason.
	
	if (!GetCharacterOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterOwner was null in PhysSliding"))
		return;
	}

	Iterations++;

	FVector OldVelocity = Velocity;
	//UE_LOG(LogTemp, Warning, TEXT("Acceleration: %s"), *Acceleration.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("OldVelocity: %s"), *OldVelocity.ToString());

	CalcVelocity(deltaTime, SlideFriction, true, GetMaxBrakingDeceleration());
	
	FFindFloorResult StartFloor;

	FindFloor(BMPCharacterOwner->GetCapsuleComponent()->GetComponentLocation(), StartFloor, false, nullptr); //pass  in our own??

	const FVector Delta = Velocity * deltaTime;
	FVector GroundDelta = ComputeGroundMovementDelta(Delta,StartFloor.HitResult /*SurfaceHitResult*/, true);
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);


	HandleImpact(Hit, deltaTime, Delta);
	//maybe compute slide vector... and then plug that ramp delta into there?
	FVector RampDelta = ComputeSlideVector(Delta, 1.f, StartFloor.HitResult.Normal, StartFloor.HitResult);
	SlideAlongSurface(RampDelta, (1.f - Hit.Time), Hit.Normal, Hit, true);
	//DrawDebugLine(GetWorld(), StartFloor.HitResult.ImpactPoint, StartFloor.HitResult.ImpactPoint + RampDelta * 100.f, FColor::Cyan, true);

	//Find if there is a ground beneath us (otherwise we're falling).
	//@TODO: If fluids, figure that out.
	FFindFloorResult Floor;
	FindFloor(BMPCharacterOwner->GetCapsuleComponent()->GetComponentLocation(), Floor, false, nullptr);
	//
	if (!Floor.IsWalkableFloor())
	{
		//UE_LOG(LogTemp, Warning, TEXT("No Floor Found"))
		EndSlide();
		SetMovementMode(MOVE_Falling);
		return;
	}
}

bool UBMPCharacterMovementComponent::IsSliding() const
{
	return BMPCharacterOwner && BMPCharacterOwner->bIsSliding;
}

bool UBMPCharacterMovementComponent::HasEnoughVelocityToEnterSlide() const
{
	return Velocity.SquaredLength() >= FMath::Square(VelocityNeededToSlide);
}

bool UBMPCharacterMovementComponent::CanSlideInCurrentState() const //
{
	if (bCanSlide == false)
	{
		return false;
	}
	return ((!IsFalling() || IsMovingOnGround()) && Velocity.Length() > 0.f); //Add simulating physics checks.
}

void UBMPCharacterMovementComponent::Slide() 
{
	if (!HasValidData()) //
	{ 
		return;
	}
	if (!BMPCharacterOwner)
	{
		return;
	}
	BMPCharacterOwner->bIsSliding = true;
	SetMovementMode(EMovementMode::MOVE_Custom, EBMPMovementMode::BMPMove_Sliding);
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
	UE_LOG(LogTemp, Warning, TEXT("EndSlide"))
	BMPCharacterOwner->bIsSliding = false;
	SetMovementMode(EMovementMode::MOVE_Walking);
}

float UBMPCharacterMovementComponent::GetForwardVelocity() const
{
	if (BMPCharacterOwner)
	{
		return FVector::DotProduct(Velocity, BMPCharacterOwner->GetActorForwardVector());
	}
	return 0.0f;
}

bool UBMPCharacterMovementComponent::CanCrouchInCurrentState() const
{
	if (!CanEverCrouch())
	{
		return false;
	}
	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics() && !IsSliding();
}

