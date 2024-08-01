


#include "BMPWeaponStateIdle.h"
#include "BMP/BMPWeapon.h"

void UBMPWeaponStateIdle::EnterState()
{
	UE_LOG(LogTemp, Display, TEXT("Enter Idle"))
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		if (OwningWeapon->WantsToFire() && OwningWeapon->CanFire())
		{
			OwningWeapon->GotoStateFiring();
		}
	}
}

void UBMPWeaponStateIdle::ExitState()
{
}

void UBMPWeaponStateIdle::HandleFireInput()
{
	//Using
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		if (OwningWeapon->CanFire())
		{
			OwningWeapon->GotoStateFiring();
		}
	}
}

void UBMPWeaponStateIdle::HandleReloadInput()
{
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		if (OwningWeapon->CanReload())
		{
			OwningWeapon->GotoStateReloading();
		}
	}
}
