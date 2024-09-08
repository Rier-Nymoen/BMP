

#pragma once

#include "CoreMinimal.h"
#include "BMPUserWidget.h"
#include "BMPKillFeed.generated.h"

/**
 * 
 */
class UTextBlock;


UCLASS()
class BMP_API UBMPKillFeed : public UBMPUserWidget
{
	GENERATED_BODY()
public:
	UBMPKillFeed();

	void AddKillToFeed();

	void SetKillerNameText();

	void SetVictimNameText();
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* KillerNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* VictimNameText;
};
