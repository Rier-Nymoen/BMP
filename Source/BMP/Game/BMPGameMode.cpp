// Copyright Epic Games, Inc. All Rights Reserved.

#include "BMPGameMode.h"
#include "BMP/BMPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "BMPPlayerState.h"
#include "BMPGameState.h"

ABMPGameMode::ABMPGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerStateClass = ABMPPlayerState::StaticClass();
	GameStateClass = ABMPGameState::StaticClass();
	ScoreToWin = 25.f;
	NumTeams = 2;
}

void ABMPGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogTemp, Display, TEXT("InitGame() - MapName: %s"), *MapName);
}

void ABMPGameMode::InitGameState()
{
	Super::InitGameState();
	ABMPGameState* BMPGameState = Cast<ABMPGameState>(GameState);

}

void ABMPGameMode::NotifyPawnDeath(const APawn* Killer, const APawn* Victim)
{
	AController* KillerController = Killer->GetController();
	AController* VictimController = Victim->GetController();

	ABMPPlayerState* VictimPlayerState = nullptr;
	ABMPPlayerState* KillerPlayerState = nullptr;

	if (VictimController)
	{
		VictimPlayerState = VictimController->GetPlayerState<ABMPPlayerState>();
	}
		
	if (KillerController)
	{
		KillerPlayerState = KillerController->GetPlayerState<ABMPPlayerState>();
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath();
		VictimPlayerState->MulticastNotifyDeath(KillerPlayerState, VictimPlayerState);
	}
	if (KillerPlayerState)
	{
		KillerPlayerState->ScoreKill();
	}
}
