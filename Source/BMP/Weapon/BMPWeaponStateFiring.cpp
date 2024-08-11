


#include "BMPWeaponStateFiring.h"
#include "BMP/BMPWeapon.h"
void UBMPWeaponStateFiring::EnterState()
{
	//UE_LOG(LogTemp, Display, TEXT("Enter Firing"))
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
	UE_LOG(LogTemp, Warning, TEXT("Timer Called"))
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
