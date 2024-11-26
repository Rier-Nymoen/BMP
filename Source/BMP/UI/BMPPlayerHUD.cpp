


#include "BMPPlayerHUD.h"

#include "BMP/BMPWeapon.h"
#include "BMP/BMPCharacter.h"
#include "Components/TextBlock.h"
#include "BMP/Game/BMPPlayerState.h"
#include "BMP/Game/BMPGameState.h"

UBMPPlayerHUD::UBMPPlayerHUD()
{
}

void UBMPPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBMPPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	APawn* OwningPawn = GetOwningPlayerPawn();

	ABMPGameState* BMPGameState = Cast<ABMPGameState>(GetWorld()->GetGameState());
}

void UBMPPlayerHUD::DisplayKillMessage(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState)
{
	UE_LOG(LogTemp, Display, TEXT(" %s Killed -> % s"), *KillerPlayerState->GetPlayerName(), *VictimPlayerState->GetPlayerName())
}
