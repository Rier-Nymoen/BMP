

#pragma once

#include "CoreMinimal.h"
#include "BMPUserWidget.h"
#include "BMPPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class BMP_API UBMPPlayerHUD : public UBMPUserWidget
{
	GENERATED_BODY()
public:

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ReserveAmmoText;
};
