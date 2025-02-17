

#pragma once

#include "CoreMinimal.h"
#include "BMPWeaponState.h"
#include "BMPWeaponStateFiring.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPWeaponStateFiring : public UBMPWeaponState
{
	GENERATED_BODY()
public:
	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleReloadInput() override;
protected:
	struct FTimerHandle TimerHandle_CheckRefire;		

	virtual void CheckRefireTimer();
};
