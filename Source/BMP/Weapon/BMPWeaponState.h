

#pragma once

#include "CoreMinimal.h"
#include "BMP/BMPState.h"
#include "BMPWeaponState.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPWeaponState : public UBMPState
{
	GENERATED_BODY()
public:
	virtual void EnterState() override;

	virtual void ExitState() override;
	
};
