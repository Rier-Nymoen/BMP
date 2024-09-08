


#include "BMPWeaponStateFiring.h"
#include "BMP/BMPWeapon.h"
void UBMPWeaponStateFiring::EnterState()
{
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		check(OwningWeapon->IsReadyToFire())
		OwningWeapon->Fire();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckRefire, this, &UBMPWeaponStateFiring::CheckRefireTimer,   OwningWeapon->GetFireRateSeconds(), true);
	}
}

void UBMPWeaponStateFiring::ExitState()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckRefire);
}

void UBMPWeaponStateFiring::HandleReloadInput()
{
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		if (OwningWeapon->CanReload())
		{
			OwningWeapon->GotoStateReloading();
		}
	}
}

void UBMPWeaponStateFiring::CheckRefireTimer()
{
	if (ABMPWeapon* OwningWeapon = GetOwningWeapon())
	{
		if (OwningWeapon->bWantsToFire && OwningWeapon->HasAmmoInMagazine())
		{

			OwningWeapon->Fire();
		}
		else if (!OwningWeapon->HasAmmoInMagazine() && OwningWeapon->GetCurrentAmmoReserves() > 0)
		{

			OwningWeapon->GotoStateReloading();
		}
		else
		{
			OwningWeapon->GotoStateIdle();
		}
	}
	
}
