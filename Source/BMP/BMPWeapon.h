

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "BMPWeapon.generated.h"

class USkeletalMeshComponent;
class ABMPCharacter;
class UInputAction;

UENUM()
enum EWeaponState : uint8
{
	Firing,
	Idle,
	Equipping,
	Unequipping
};

UCLASS()
class BMP_API ABMPWeapon : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABMPWeapon();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void Interact(ABMPCharacter* TargetCharacter) override;
	virtual void OnInteractionRangeEntered(ABMPCharacter* TargetCharacter) override;
	virtual void OnInteractionRangeExited(ABMPCharacter* TargetCharacter) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	////first person view
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh1P;

	//World view
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* Mesh3P;

	class UBMPPickupComponent* PickUpComponent;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void OnEquip(ABMPCharacter* NewCharacter);

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	virtual void HandleFireInput();

	virtual void Fire();

	virtual void FireHitscan();

	virtual void FireProjectile();

	UFUNCTION(Server, Reliable)
	void ServerFireProjectile(FVector AimLocation, FRotator AimRotation);

	UFUNCTION(Server, Reliable)
	void ServerFireHitscan(FVector StartTrace, FVector EndTrace);

	bool bWantsToFire;

protected:
	UFUNCTION()
	void OnRep_Character();

	UPROPERTY(ReplicatedUsing=OnRep_Character)
	ABMPCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ABMPProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Hitscan")
	float HitscanRange;

	void BindInput(ABMPCharacter* TargetCharacter);
	//Add UnbindInput()
};
