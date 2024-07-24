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
