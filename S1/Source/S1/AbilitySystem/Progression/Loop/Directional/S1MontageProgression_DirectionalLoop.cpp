// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/Loop/Directional/S1MontageProgression_DirectionalLoop.h"

void US1MontageProgression_DirectionalLoop::SetDirection(ES1Direction InDirection)
{
	if (const TObjectPtr<UAnimMontage>* Found = DirectionalMontages.Find(InDirection))
	{
		SectionMontage = *Found;
	}
}