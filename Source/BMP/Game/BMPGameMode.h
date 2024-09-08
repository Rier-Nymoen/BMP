// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BMPGameMode.generated.h"

UCLASS(MinimalAPI)
class ABMPGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABMPGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void InitGameState() override;

	virtual void NotifyPawnDeath(const APawn* Killer, const APawn* Victim);

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 ScoreToWin;
	
	int32 NumTeams;
};



