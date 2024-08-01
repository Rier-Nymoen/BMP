


#include "BMPPlayerController.h"
#include "UI/BMPPlayerHUD.h"

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
