


#include "BMPAttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UBMPAttributeSetBase::UBMPAttributeSetBase()
{
}

void UBMPAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UBMPAttributeSetBase, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UBMPAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always)
}

void UBMPAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UBMPAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	AActor* EffectInstigator = Context.GetInstigator();
	AActor* TargetActor = nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		//assign
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	}
	//Because Im modifying the health directly, this change doesnt generate GEModData. Its not using a Gameplay Effect Spec to do it.
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float OldHealth = GetHealth();

		SetHealth(GetHealth() - GetDamage());
		OnRep_Health(OldHealth);
		SetDamage(0.f);
	}
}

void UBMPAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBMPAttributeSetBase, Health, OldHealth)
}

void UBMPAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBMPAttributeSetBase, MaxHealth, OldMaxHealth)
}
