


#include "BMPPlayerState.h"
#include "Net/UnrealNetwork.h"

ABMPPlayerState::ABMPPlayerState()
{

}

void ABMPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABMPPlayerState, Ping);
}
