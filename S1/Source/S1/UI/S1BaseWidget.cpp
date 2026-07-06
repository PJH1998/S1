// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1BaseWidget.h"

FGameplayTag US1BaseWidget::AppendGenderSuffix(const FGameplayTag& BaseTag, EPlayerGender Gender)
{
	if (false == BaseTag.IsValid())
	{
		return BaseTag;
	}

	const TCHAR* Suffix = (Gender == EPlayerGender::Female) ? TEXT(".Female") : TEXT(".Male");
	return FGameplayTag::RequestGameplayTag(FName(*(BaseTag.GetTagName().ToString() + Suffix)));
}

