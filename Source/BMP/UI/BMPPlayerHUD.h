

#pragma once

#include "CoreMinimal.h"
#include "BMPUserWidget.h"
#include "BMPPlayerHUD.generated.h"

/**
 * 
 */
class UTextBlock;
class ABMPPlayerState;

UCLASS()
class BMP_API UBMPPlayerHUD : public UBMPUserWidget
{
	GENERATED_BODY()
public:
	UBMPPlayerHUD();

	void DisplayKillMessage(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CurrentAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ReserveAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HealthText;
};
