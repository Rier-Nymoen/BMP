

#pragma once

#include "CoreMinimal.h"
#include "BMP/BMPState.h"
#include "BMP/BMPWeapon.h"
#include "BMPWeaponState.generated.h"
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BMP_API UBMPWeaponState : public UBMPState
{
	GENERATED_BODY()
public:
	virtual void EnterState() override;

	virtual void ExitState() override;
	
	virtual void HandleFireInput();

	virtual void HandleReloadInput();

protected:
	virtual class ABMPWeapon* GetOwningWeapon() { return Cast<class ABMPWeapon>(GetOwner()); }
};
