


#include "BMPCharacterMovementComponent.h"
#include "BMPCharacter.h"
#include "Components/CapsuleComponent.h"

UBMPCharacterMovementComponent::UBMPCharacterMovementComponent()
{
	ForwardVelocityNeededToSlide = 732.f;
	VelocityNeededToSlide = 100.f;
	bCanSlide = true;
	bWantsToSlide = false;
	MaxSlidingSpeed = MaxWalkSpeed;
	BrakingDecelerationSliding = 100.f;
	SlideFriction = 0.1f;
	SlideGravityCoefficient = 1.f;
}

void UBMPCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UE_LOG(LogTemp, Warning, TEXT("Tick MoveSpeed: %f"), Velocity.Length())
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
	if (!bIsSliding && bWantsToCrouch && CanSlideInCurrentState() /*&& HasEnoughVelocityToEnterSlide()*/) 
	{
		Slide();
	}
	else if (bIsSliding && (!bWantsToCrouch || !CanSlideInCurrentState()))
	{
		EndSlide();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

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
	
	if (!GetCharacterOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterOwner was null in PhysSliding"))
		return;
	}

	FFindFloorResult OldFloor = CurrentFloor;
	UCapsuleComponent* CapsuleComponent = GetCharacterOwner()->GetCapsuleComponent();
	FindFloor(CapsuleComponent->GetComponentLocation(), CurrentFloor, false, nullptr);

	FVector OldVelocity = Velocity;
	//MaintainHorizontalGroundVelocity();

	FVector GravityProjection = FVector::VectorPlaneProject(FVector::UpVector, CurrentFloor.HitResult.ImpactNormal); //may need dot product involved for keeping momentum.

	FVector GravityForce = GravityProjection * GetGravityZ() * SlideGravityCoefficient;
	
	GravityForce.Z = 0.f;
	Velocity += GravityForce * deltaTime;
	
	float DotProduct = FMath::Abs(Acceleration.GetSafeNormal() | CapsuleComponent->GetRightVector());
	if (DotProduct > SlideStrafeControl)
	{
		Acceleration *= DotProduct;
		//UE_LOG(LogTemp, Warning, TEXT("GravityForce: %f"), GravityForce.Length())
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	//HelperDrawVectorFromPlayer(Acceleration, Acceleration.Length(), FColor::Red, false, deltaTime + 0.1);
	HelperDrawVectorFromPlayer(GravityForce, GravityForce.Length(), FColor::Green, false, deltaTime + 0.1, FVector::ZeroVector);

	//UE_LOG(LogTemp, Warning, TEXT("Acceleration: %s"), *Acceleration.ToString())
	CalcVelocity(deltaTime, SlideFriction, true, GetMaxBrakingDeceleration());

	//UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString())
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, deltaTime + 0.05f, FColor::Cyan, FString::Printf(TEXT("Velocity: %s"), *Velocity.ToString()), true);

	const FVector Delta = Velocity * deltaTime;
	FVector GroundDelta = ComputeGroundMovementDelta(Delta, CurrentFloor.HitResult , CurrentFloor.bLineTrace);

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(GroundDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
	HandleImpact(Hit, deltaTime, GroundDelta);
	FVector RampDelta = ComputeSlideVector(GroundDelta, 1.f, CurrentFloor.HitResult.Normal, CurrentFloor.HitResult);
	SlideAlongSurface(RampDelta, (1.f - Hit.Time), Hit.Normal, Hit, true);

	FindFloor(CapsuleComponent->GetComponentLocation(), CurrentFloor, false, nullptr);

	if (!CurrentFloor.IsWalkableFloor())
	{
		EndSlide();
		SetMovementMode(MOVE_Falling);
		return;
	}
	
	//if (IsMovingOnGround())
	//{
	//	MaintainHorizontalGroundVelocity();
	//}
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
	return ((!IsFalling() || IsMovingOnGround()) /*&& Velocity.SquaredLength() > 0.f*/); //Add simulating physics checks.
}

void UBMPCharacterMovementComponent::Slide() 
{
	//UE_LOG(LogTemp, Warning, TEXT("EnterSlide"))

	if (!HasValidData()) //
	{ 
		return;
	}
	if (!BMPCharacterOwner)
	{
		return;
	}

	if (!WasFalling())
	{
		Velocity += Velocity.GetSafeNormal2D() * SlideImpulse;
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

	//UE_LOG(LogTemp, Warning, TEXT("EndSlide"))
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

bool UBMPCharacterMovementComponent::WasFalling()
{
	return PrevMovementMode == EMovementMode::MOVE_Falling;
}

bool UBMPCharacterMovementComponent::IsMovingOnGround() const
{
	return ((MovementMode == MOVE_Walking) || (MovementMode == MOVE_NavWalking) || (CustomMovementMode == BMPMove_Sliding)) && UpdatedComponent;
}

bool UBMPCharacterMovementComponent::CanCrouchInCurrentState() const
{
	if (!CanEverCrouch())
	{
		return false;
	}
	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics() && !IsSliding();
}

float UBMPCharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return IsCrouching() ? MaxWalkSpeedCrouched : MaxWalkSpeed;
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return GetCustomMaxSpeed();
	case MOVE_None:
	default:
		return 0.f;
	}
}

float UBMPCharacterMovementComponent::GetCustomMaxSpeed() const
{
	switch (CustomMovementMode)
	{
	case EBMPMovementMode::BMPMove_None:
		return 0.f;
		break;
	case EBMPMovementMode::BMPMove_Sliding:
		return MaxSlidingSpeed;
		break;
	default:
		return 0.f;
		break;
	}
}

void UBMPCharacterMovementComponent::HelperDrawVectorFromPlayer(FVector Vector, float Length, FColor Color, bool bPersistent, float LifeTime, FVector Offset) const
{
	FVector StartTrace = CharacterOwner->GetCapsuleComponent()->GetComponentLocation() + Offset;
	FVector EndTrace = StartTrace + (Vector * Length);
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, Color, bPersistent, LifeTime);
	return;
}

void UBMPCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	PrevMovementMode = PreviousMovementMode;
	PrevBMPMovementMode = (EBMPMovementMode)PreviousCustomMode;
	//UE_LOG(LogTemp, Warning, TEXT("PrevMovementMode: %d"), PrevMovementMode);

	//UE_LOG(LogTemp, Warning, TEXT("PrevBMPMovementNode: %d"), PrevBMPMovementMode);
}

