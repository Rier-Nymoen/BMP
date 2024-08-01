

#pragma once

#include "CoreMinimal.h"
#include "BMPWeaponState.h"
#include "BMPWeaponStateReloading.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPWeaponStateReloading : public UBMPWeaponState
{
	GENERATED_BODY()

	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void ReloadWeapon();

	FTimerHandle TimerHandle_Reload;
};
