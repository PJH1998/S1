// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProgressBar/ExpBar/S1ExpBar.h"
#include "Player/S1PlayerState.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"
#include "Components/TextBlock.h"

US1ExpBar::US1ExpBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1ExpBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (AS1PlayerState* PS = Cast<AS1PlayerState>(GetOwningPlayerState()))
	{
		PlayerSet = PS->GetS1PlayerSet();
	}

	if (PlayerSet == nullptr)
	{
		UE_LOG(LogWindows, Error, TEXT("Can't Search PlayerSet"));
		return;
	}

	SetValue(PlayerSet->GetCurrentXP(), PlayerSet->GetMaxXP());
	LerpValue = CurrentValue;
	LerpSpeed = 0.4f;
	UpdateLevelText(FMath::RoundToInt(PlayerSet->GetLevel()));
}

void US1ExpBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// PlayerState 복제가 늦게 도착할 수 있으므로 null이면 재조회
	if (PlayerSet == nullptr)
	{
		if (AS1PlayerState* PS = Cast<AS1PlayerState>(GetOwningPlayerState()))
		{
			PlayerSet = PS->GetS1PlayerSet();
		}

		if (PlayerSet == nullptr)
		{
			return;
		}
	}

	SetValue(PlayerSet->GetCurrentXP(), PlayerSet->GetMaxXP());

	if (CurrentValue >= LerpValue)
	{
		LerpValue = CurrentValue;
	}
	else
	{
		const float LerpDelta = FMath::Max(MaxValue, 1.f) * LerpSpeed * InDeltaTime;
		LerpValue = FMath::Max(CurrentValue, LerpValue - LerpDelta);
	}

	Bind_ShaderResource();
	UpdateLevelText(FMath::RoundToInt(PlayerSet->GetLevel()));
}

void US1ExpBar::Bind_ShaderResource()
{
	if (Fill_MID == nullptr)
	{
		return;
	}

	Fill_MID->SetScalarParameterValue(FillRatioParameterName, GetFillRatio(CurrentValue));
	Fill_MID->SetScalarParameterValue(LerpRatioParameterName, GetFillRatio(LerpValue));
}

void US1ExpBar::UpdateLevelText(int32 InLevel)
{
	if (Text_Level == nullptr)
	{
		return;
	}

	Text_Level->SetText(FText::Format(NSLOCTEXT("ExpBar", "LevelFormat", "Lv. {0}"), FText::AsNumber(InLevel)));
}
