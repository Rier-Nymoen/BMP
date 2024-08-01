


#include "BMPPlayerHUD.h"

#include "BMP/BMPWeapon.h"
#include "BMP/BMPCharacter.h"
#include "Components/TextBlock.h"

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
			CurrentAmmoText->SetText(FText::AsNumber(OwningCharacterWeapon->GetCurrentAmmo()));
			ReserveAmmoText->SetText(FText::AsNumber(OwningCharacterWeapon->GetCurrentAmmoReserves()));
		}
	}
	
	
}
