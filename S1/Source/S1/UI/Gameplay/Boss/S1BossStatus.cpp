// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Boss/S1BossStatus.h"
#include "AbilitySystem/Attributes/S1BossSet.h"
#include "Character/Boss/S1BossBase.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "System/S1CombatFeedbackSubsystem.h"

US1BossStatus::US1BossStatus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1BossStatus::SetBoss(AS1BossBase* InBoss)
{
	Boss = IsValid(InBoss) ? InBoss : nullptr;
	BossSet = Boss ? Boss->GetS1BossSet() : nullptr;
	PrevHealthForDamageNumber = -1.f;

	if (BossSet == nullptr)
	{
		if (Text_Name != nullptr)
		{
			Text_Name->SetText(FText::GetEmpty());
		}
		UpdateLineCountText(0);

		SetValue(0.f, 1.f);
		LerpValue = CurrentValue;
		Bind_ShaderResource();
		return;
	}

	if (Text_Name != nullptr)
	{
		Text_Name->SetText(FText::FromName(Boss->GetBossName()));
	}

	SetValue(BossSet->GetHealth(), BossSet->GetMaxHealth());
	LerpValue = CurrentValue;
	Bind_ShaderResource();
}

void US1BossStatus::NativeConstruct()
{
	Super::NativeConstruct();

	LerpValue = CurrentValue;
	LerpSpeed = 0.4f;
	Bind_ShaderResource();
}

void US1BossStatus::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(Boss) || !IsValid(BossSet))
	{
		SetBoss(nullptr);
		return;
	}

	const float NewHealth = BossSet->GetHealth();
	SetValue(NewHealth, BossSet->GetMaxHealth());

	if (PrevHealthForDamageNumber >= 0.f && NewHealth < PrevHealthForDamageNumber)
	{
		const int32 DamageAmount = FMath::RoundToInt(PrevHealthForDamageNumber - NewHealth);
		if (DamageAmount > 0)
		{
			if (UWorld* World = GetWorld())
			{
				if (US1CombatFeedbackSubsystem* CombatFeedback = World->GetSubsystem<US1CombatFeedbackSubsystem>())
				{
					CombatFeedback->ShowDamageNumber(DamageAmount, Boss->GetActorLocation());
				}
			}
		}
	}
	PrevHealthForDamageNumber = NewHealth;

	if (CurrentValue >= LerpValue)
	{
		LerpValue = CurrentValue;
	}
	else if (GetLineCount(CurrentValue) != GetLineCount(LerpValue))
	{
		// 라인 경계를 넘으면 트레일을 새 라인 기준으로 리셋(이전 라인 값이 새 라인 비율 계산에 새어 들어가 투명하게 보이는 것 방지).
		LerpValue = CurrentValue;
	}
	else
	{
		const float LerpDelta = FMath::Max(MaxValue, 1.f) * LerpSpeed * InDeltaTime;
		LerpValue = FMath::Max(CurrentValue, LerpValue - LerpDelta);
	}

	Bind_ShaderResource();
}

void US1BossStatus::Bind_ShaderResource()
{
	if (Fill_MID == nullptr)
	{
		return;
	}

	const int32 CurrentLineCount = GetLineCount(CurrentValue);
	const int32 NextLineCount = CurrentLineCount - 1;

	Fill_MID->SetScalarParameterValue(FillRatioParameterName, GetLineFillRatio(CurrentValue, CurrentLineCount));
	Fill_MID->SetScalarParameterValue(LerpRatioParameterName, GetLineFillRatio(LerpValue, CurrentLineCount));
	Fill_MID->SetVectorParameterValue(CurrentColorParameterName, GetLineColor(CurrentLineCount));
	Fill_MID->SetVectorParameterValue(NextColorParameterName, NextLineCount > 0 ? GetLineColor(NextLineCount) : FLinearColor::Transparent);

	UpdateLineCountText(CurrentLineCount);
}

int32 US1BossStatus::GetLineCount(float InValue) const
{
	if (HealthPerLine <= KINDA_SMALL_NUMBER)
	{
		return 0;
	}

	const float ClampedValue = FMath::Clamp(InValue, 0.f, MaxValue);
	return FMath::CeilToInt(ClampedValue / HealthPerLine);
}

float US1BossStatus::GetLineFillRatio(float InValue, int32 InLineCount) const
{
	if (HealthPerLine <= KINDA_SMALL_NUMBER || InLineCount <= 0)
	{
		return 0.f;
	}

	const float LineMinValue = (InLineCount - 1) * HealthPerLine;
	const float ValueInLine = FMath::Clamp(InValue - LineMinValue, 0.f, HealthPerLine);
	return ValueInLine / HealthPerLine;
}

FLinearColor US1BossStatus::GetLineColor(int32 InLineCount) const
{
	static const FLinearColor LineColors[] =
	{
		FLinearColor(1.f, 0.15f, 0.f, 1.f),
		FLinearColor(0.f, 0.65f, 1.f, 1.f),
		FLinearColor(0.45f, 1.f, 0.1f, 1.f),
		FLinearColor(1.f, 0.85f, 0.f, 1.f),
	};

	const int32 MaxLineCount = GetLineCount(MaxValue);
	if (InLineCount <= 0 || MaxLineCount <= 0)
	{
		return FLinearColor::Transparent;
	}

	const int32 ColorIndex = FMath::Clamp(MaxLineCount - InLineCount, 0, MaxLineCount - 1) % UE_ARRAY_COUNT(LineColors);
	return LineColors[ColorIndex];
}

void US1BossStatus::UpdateLineCountText(int32 InLineCount)
{
	if (Text_Num == nullptr)
	{
		return;
	}

	Text_Num->SetText(FText::Format(NSLOCTEXT("BossStatus", "LineCountFormat", "x{0}"), FText::AsNumber(FMath::Max(InLineCount, 0))));
}
