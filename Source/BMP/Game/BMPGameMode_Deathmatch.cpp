#include "BMPGameMode_Deathmatch.h"
#include "BMP/BMPCharacter.h"
#include "BMPPlayerState.h"
#include "BMPGameState.h"

ABMPGameMode_Deathmatch::ABMPGameMode_Deathmatch()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerStateClass = ABMPGameMode_Deathmatch::StaticClass();
	GameStateClass = ABMPGameState::StaticClass();
}

void ABMPGameMode_Deathmatch::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogTemp, Display, TEXT("InitGame() - MapName: %s"), *MapName);
}

void ABMPGameMode_Deathmatch::InitGameState()
{
	Super::InitGameState();
}
