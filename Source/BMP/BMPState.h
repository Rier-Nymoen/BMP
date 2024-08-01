

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BMPState.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPState : public UActorComponent
{
	GENERATED_BODY()
public:
	virtual void EnterState();

	virtual void ExitState();
};
