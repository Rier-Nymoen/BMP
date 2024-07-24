#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ABMPCharacter;

class BMP_API IInteractableInterface
{
	GENERATED_BODY()
public:
	virtual void Interact(ABMPCharacter* TargetCharacter) = 0;

	virtual void OnInteractionRangeEntered(ABMPCharacter* TargetCharacter) = 0;
	virtual void OnInteractionRangeExited(ABMPCharacter* TargetCharacter) = 0;
};