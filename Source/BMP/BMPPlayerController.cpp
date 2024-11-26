#include "BMPPlayerController.h"
#include "Game/BMPPlayerState.h"
#include "UI/BMPPlayerHUD.h"

void ABMPPlayerController::DisplayKillMessage(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState)
{
	//if (PlayerHUD)
	//{
	//	PlayerHUD->DisplayKillMessage(KillerPlayerState, VictimPlayerState);
	//}
}

void ABMPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("%s: PlayerController()"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"));
	if (IsLocalPlayerController())
	{
		UE_LOG(LogTemp, Display, TEXT("Local Controller."))
		if (PlayerHUDClass)
		{

			PlayerHUD = CreateWidget<UBMPPlayerHUD>(this, PlayerHUDClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}
	}
}
