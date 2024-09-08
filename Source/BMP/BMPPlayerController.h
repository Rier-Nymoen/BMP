

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BMPPlayerController.generated.h"

class UBMPPlayerHUD;
class ABMPPlayerState;
/**
 * 
 */
UCLASS()
class BMP_API ABMPPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UBMPPlayerHUD* GetPlayerHUD() const { return PlayerHUD; }

	//maybe bind an event instead.
	void DisplayKillMessage(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UBMPPlayerHUD> PlayerHUDClass;

	UBMPPlayerHUD* PlayerHUD;

};
