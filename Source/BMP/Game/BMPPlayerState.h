

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

protected:
	UPROPERTY(Transient, Replicated)
	int32 Ping;

};
