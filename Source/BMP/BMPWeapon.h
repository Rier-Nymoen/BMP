

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "AbilitySystemInterface.h"

#include "BMPWeapon.generated.h"

class USkeletalMeshComponent;
class ABMPCharacter;
class UInputAction;


UCLASS()
class BMP_API ABMPWeapon : public AActor, public IInteractableInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABMPWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	void OnEquip(ABMPCharacter* NewCharacter);

protected:

	virtual void StartFire();

	UFUNCTION(Server, Reliable)
	virtual void ServerStartFire();

	virtual void StopFire();

	UFUNCTION(Server, Reliable)
	virtual void ServerStopFire();

	virtual void Fire();

	virtual void PlayFiringEffects();

	virtual void FireHitscan();

	virtual void FireProjectile();

	UFUNCTION(Server, Reliable)
	void ServerFireProjectile(FVector AimLocation, FRotator AimRotation);

	UFUNCTION(Server, Reliable)
	void ServerFireHitscan(FVector StartTrace, FVector EndTrace);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ABMPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	float HitscanRange;

	bool bWantsToFire;

protected:
	UFUNCTION()
	void OnRep_Character();

	UPROPERTY(ReplicatedUsing=OnRep_Character)
	ABMPCharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* StopFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	void BindInput(ABMPCharacter* TargetCharacter);
	//Add UnbindInput()

	virtual void GotoState(class UBMPWeaponState* NewState);
	void GotoStateIdle();
	void GotoStateReloading();
	void GotoStateFiring();

	class UBMPWeaponState* CurrentState;
	class UBMPWeaponStateIdle* IdleState;
	class UBMPWeaponStateFiring* FiringState;
	class UBMPWeaponStateReloading* ReloadingState;

	friend class UBMPWeaponState;
	friend class UBMPWeaponStateIdle;
	friend class UBMPWeaponStateFiring;
	friend class UBMPWeaponStateReloading;

	//Probably replicate this?
	UPROPERTY(VisibleAnywhere)
	float LastTimeFiredSeconds;

	UPROPERTY(EditAnywhere)
	float FireRateSeconds;

	UPROPERTY(Replicated, EditAnywhere, Category = Ammo)
	int32 MaxAmmoReserves;

	UPROPERTY(Replicated, EditAnywhere, Category = Ammo)
	int32 CurrentAmmoReserves;

	UPROPERTY(Replicated, EditAnywhere, Category = Ammo)
	int32 CurrentAmmo;

	UPROPERTY(Replicated, EditAnywhere, Category = Ammo)
	int32 MagazineSize;

	UPROPERTY(Replicated, EditAnywhere, Category = Ammo)
	int32 AmmoCost;

	UPROPERTY(EditAnywhere)
	float ReloadTimeSeconds;
public:

	int32 GetCurrentAmmo() const { return CurrentAmmo; }
	int32 GetCurrentAmmoReserves() const { return CurrentAmmoReserves; }

	bool HasAmmoInMagazine() const  { return CurrentAmmo > 0; }

	float GetFireRateSeconds() const { return FireRateSeconds; }

	//Doesn't have a margin of error. - Having a slow reload speed is one way.
	bool IsReadyToFire() const;

	bool WantsToFire() const { return bWantsToFire; }

	UFUNCTION(BlueprintCallable)
	virtual void AddAmmo(float Amount);

	UFUNCTION(BlueprintCallable)
	virtual void AddAmmoReserves(float Amount);

	UFUNCTION()
	bool CanReload() const;

	bool CanFire() const;
	
protected:

	virtual void HandleReloadInput();

	virtual void ReloadWeapon();

	UFUNCTION(Server, Reliable)
	virtual void ServerReloadWeapon();

	UFUNCTION(Server, Reliable)
	virtual void ServerProcessHit(const FHitResult& HitResult);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundCue* FireSoundCue;

public:
	class UAbilitySystemComponent* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	class UBaseSet* BaseSet;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite	)
	TSubclassOf<class UGameplayEffect> DamageEffect;
};
