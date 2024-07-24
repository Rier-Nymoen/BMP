

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BMPState.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPState : public UObject
{
	GENERATED_BODY()
public:
	virtual void EnterState();

	virtual void ExitState();
};
