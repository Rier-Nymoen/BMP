

#pragma once
#include "BMP/BMPAttributeSetBase.h"


struct FBMPDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition DamageDef;
	FGameplayEffectAttributeCaptureDefinition HealthDef;

	FBMPDamageStatics()
	{
		DamageDef = FGameplayEffectAttributeCaptureDefinition(
		UBMPAttributeSetBase::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);

		HealthDef = FGameplayEffectAttributeCaptureDefinition(
			UBMPAttributeSetBase::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static FBMPDamageStatics& BMPDamageStatics()
{
	static FBMPDamageStatics Statics;
	return Statics;
}

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BMPDamageExecutionCalculation.generated.h"
/**
 * 
 */
UCLASS()
class BMP_API UBMPDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UBMPDamageExecutionCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
