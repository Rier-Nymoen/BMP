#include "BMPPickupComponent.h"

UBMPPickupComponent::UBMPPickupComponent()
{
	bHiddenInGame = false;
	SphereRadius = 150.f;
	ShapeColor = FColor(0.f, 50.f, 210.f);
}

void UBMPPickupComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBMPPickupComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UBMPPickupComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
