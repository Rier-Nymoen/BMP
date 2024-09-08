

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BMPPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API ABMPPlayerState : public APlayerState
{
	GENERATED_BODY()


public:
	ABMPPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UFUNCTION()
	int32 GetPing() const { return Ping; }

	UFUNCTION()
	int32 GetKills() const { return Kills; }

	UFUNCTION() 
	int32 GetDeaths() const { return Deaths; }

	UFUNCTION()
	virtual void ScoreKill();

	UFUNCTION()
	virtual void ScoreDeath();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastNotifyDeath(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState);


protected:
	UPROPERTY(Transient, Replicated)
	int32 Ping;

	UPROPERTY(Transient, Replicated)
	int32 Kills;

	UPROPERTY(Transient, Replicated)
	int32 Deaths;

};
