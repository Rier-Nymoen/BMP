


#include "BMPDamageExecutionCalculation.h"

UBMPDamageExecutionCalculation::UBMPDamageExecutionCalculation()
{
	RelevantAttributesToCapture.Add(FBMPDamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(FBMPDamageStatics().DamageDef);
}
//testing purposes
void UBMPDamageExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	//Grab the tags from source and target
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;

	float Health = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BMPDamageStatics().HealthDef, EvaluateParameters, Health);
	Health--;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBMPAttributeSetBase::GetHealthAttribute(), EGameplayModOp::Additive, Health));
}
