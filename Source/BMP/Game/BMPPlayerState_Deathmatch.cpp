


#include "BMPPlayerState_Deathmatch.h"
#include "Net/UnrealNetwork.h"

ABMPPlayerState_Deathmatch::ABMPPlayerState_Deathmatch()
{
}

void ABMPPlayerState_Deathmatch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABMPPlayerState_Deathmatch, NumKills)
	DOREPLIFETIME(ABMPPlayerState_Deathmatch, BulletsFired)
	DOREPLIFETIME(ABMPPlayerState_Deathmatch, BulletsHit)
}
