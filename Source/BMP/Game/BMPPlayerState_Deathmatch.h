

#pragma once

#include "CoreMinimal.h"
#include "BMPPlayerState.h"
#include "BMPPlayerState_Deathmatch.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API ABMPPlayerState_Deathmatch : public ABMPPlayerState
{
	GENERATED_BODY()

public:
	ABMPPlayerState_Deathmatch();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	int32 GetNumKills() const { return NumKills; }

	int32 GetBulletsFired() const { return BulletsFired; }

	int32 GetBulletsHit() const { return BulletsHit; }
protected:
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	UPROPERTY(Transient, Replicated)
	int32 BulletsFired;

	UPROPERTY(Transient, Replicated)
	int32 BulletsHit;
};
