

#pragma once

#include "CoreMinimal.h"
#include "BMPWeaponState.h"
#include "BMPWeaponStateIdle.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPWeaponStateIdle : public UBMPWeaponState
{
	GENERATED_BODY()
public:
	virtual void EnterState() override;

	virtual void ExitState() override;

	virtual void HandleFireInput();

	virtual void HandleReloadInput() override;

};
