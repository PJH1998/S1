// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ProgressBar/HPBar/S1HPBar.h"
#include "Player/S1PlayerState.h"
#include "AbilitySystem/Attributes/Player/S1PlayerSet.h"

US1HPBar::US1HPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1HPBar::NativeConstruct()
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

	SetValue(PlayerSet->GetHealth(), PlayerSet->GetMaxHealth());
	LerpValue = CurrentValue;
	LerpSpeed = 0.4f;
}

void US1HPBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

	SetValue(PlayerSet->GetHealth(), PlayerSet->GetMaxHealth());

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
}

void US1HPBar::Bind_ShaderResource()
{
	if (Fill_MID == nullptr)
	{
		return;
	}

	Fill_MID->SetScalarParameterValue(FillRatioParameterName, GetFillRatio(CurrentValue));
	Fill_MID->SetScalarParameterValue(LerpRatioParameterName, GetFillRatio(LerpValue));
}
