


#include "BMPWeaponStateReloading.h"
#include "BMP/BMPWeapon.h"

void UBMPWeaponStateReloading::EnterState()
{
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		if (OwningWeapon->CanReload())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Reload, this, &UBMPWeaponStateReloading::ReloadWeapon, OwningWeapon->ReloadTimeSeconds, false);
		}
		else
		{
			OwningWeapon->GotoStateIdle();
		}
	}

}

void UBMPWeaponStateReloading::ExitState()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Reload);
	//Cancel the weapon reload.
}

void UBMPWeaponStateReloading::ReloadWeapon()
{


	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		OwningWeapon->ReloadWeapon();
		OwningWeapon->GotoStateIdle();
	}
}
