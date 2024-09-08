

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BMPGameState.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API ABMPGameState : public AGameState
{
	GENERATED_BODY()
public:
	ABMPGameState();

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<int32> TeamScores;
	
};
