


#include "BMPPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "BMP/BMPPlayerController.h"

ABMPPlayerState::ABMPPlayerState()
{

}

void ABMPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABMPPlayerState, Ping)
	DOREPLIFETIME(ABMPPlayerState, Kills)
	DOREPLIFETIME(ABMPPlayerState, Deaths)
}

void ABMPPlayerState::ScoreKill()
{
	Kills++;
}

void ABMPPlayerState::ScoreDeath()
{
	Deaths++;
}

void ABMPPlayerState::MulticastNotifyDeath_Implementation(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState)
{
	UE_LOG(LogTemp, Display, TEXT("%s: MulticastNotifyDeath()"), GetNetMode() == ENetMode::NM_Client ? TEXT("Client") : TEXT("Server"))
	for (FConstPlayerControllerIterator Iter = GetWorld()->GetPlayerControllerIterator(); Iter; Iter++)
	{
		ABMPPlayerController* PlayerController = Cast<ABMPPlayerController>(*Iter);
		//Need IsLocal, otherwise server would call this on each server PC.
		if (PlayerController && PlayerController->IsLocalController())
		{
			PlayerController->DisplayKillMessage(KillerPlayerState, VictimPlayerState);
		}
	}
}
