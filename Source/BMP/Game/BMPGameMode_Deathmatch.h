

#pragma once

#include "CoreMinimal.h"
#include "BMPGameMode.h"
#include "BMPGameMode_Deathmatch.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API ABMPGameMode_Deathmatch : public ABMPGameMode
{
	GENERATED_BODY()

public:
	ABMPGameMode_Deathmatch();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

protected:
};
