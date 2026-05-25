// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/Boss/S1BossStatus.h"
#include "AbilitySystem/Attributes/S1BossSet.h"
#include "Character/Boss/S1BossBase.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

US1BossStatus::US1BossStatus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1BossStatus::SetBoss(AS1BossBase* InBoss)
{
	Boss = IsValid(InBoss) ? InBoss : nullptr;
	BossSet = Boss ? Boss->GetS1BossSet() : nullptr;

	if (BossSet == nullptr)
	{
		if (Text_Name != nullptr)
		{
			Text_Name->SetText(FText::GetEmpty());
		}

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
	LerpSpeed = 25.f;
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

	BossSet->SetHealth(BossSet->GetHealth() - 2.f * InDeltaTime);

	SetValue(BossSet->GetHealth(), BossSet->GetMaxHealth());

	if (CurrentValue >= LerpValue)
	{
		LerpValue = CurrentValue;
	}
	else
	{
		LerpValue = FMath::Max(CurrentValue, LerpValue - LerpSpeed * InDeltaTime);
	}

	Bind_ShaderResource();
}

void US1BossStatus::Bind_ShaderResource()
{
	if (Fill_MID == nullptr)
	{
		return;
	}

	Fill_MID->SetScalarParameterValue(FillRatioParameterName, GetFillRatio(CurrentValue));
	Fill_MID->SetScalarParameterValue(LerpRatioParameterName, GetFillRatio(LerpValue));
}
