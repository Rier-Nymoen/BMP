


#include "BMPPlayerHUD.h"

#include "BMP/BMPWeapon.h"
#include "BMP/BMPCharacter.h"
#include "Components/TextBlock.h"
#include "BMP/Game/BMPPlayerState.h"

UBMPPlayerHUD::UBMPPlayerHUD()
{
}

void UBMPPlayerHUD::DisplayKillMessage(const ABMPPlayerState* KillerPlayerState, const ABMPPlayerState* VictimPlayerState)
{
	UE_LOG(LogTemp, Display, TEXT(" %s -> % s"), *KillerPlayerState->GetPlayerName(), *VictimPlayerState->GetPlayerName())
}

void UBMPPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBMPPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	APawn* OwningPawn = GetOwningPlayerPawn();

	if (ABMPCharacter* OwningCharacter = Cast<ABMPCharacter>(OwningPawn))
	{
		if (ABMPWeapon* OwningCharacterWeapon = Cast<ABMPWeapon>(OwningCharacter->GetEquippedWeapon()))
		{
			if (CurrentAmmoText)
			{
				CurrentAmmoText->SetText(FText::FromString(FString("Current Ammo: ") + FString::SanitizeFloat(OwningCharacterWeapon->GetCurrentAmmo())));
			}
			if (ReserveAmmoText)
			{
				ReserveAmmoText->SetText(FText::FromString(FString("Reserve Ammo: ") + FString::SanitizeFloat(OwningCharacterWeapon->GetCurrentAmmoReserves())));
			}
		}
		if (HealthText)
		{
			HealthText->SetText(FText::FromString(FString("Health: ") + FString::SanitizeFloat(OwningCharacter->GetHealth())));
		}
	}	
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
