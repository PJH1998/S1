// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ProgressBar/HPBar/S1MonsterHPBar.h"

#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "Animation/WidgetAnimation.h"
#include "Character/S1Monster.h"
#include "Components/TextBlock.h"

US1MonsterHPBar::US1MonsterHPBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1MonsterHPBar::SetMonster(AS1Monster* InMonster)
{
	Monster = IsValid(InMonster) ? InMonster : nullptr;

	if (!IsValid(Monster))
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
		Text_Name->SetText(Monster->GetMonsterName());
	}

	if (const US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(Monster->GetAbilitySystemComponent()))
	{
		if (const US1AttributeSet* MonsterAttributeSet = Cast<US1AttributeSet>(ASC->GetAttributeSet(US1AttributeSet::StaticClass())))
		{
			SetValue(MonsterAttributeSet->GetHealth(), MonsterAttributeSet->GetMaxHealth());
			LerpValue = CurrentValue;
			Bind_ShaderResource();
			return;
		}
	}

	SetValue(0.f, 1.f);
	LerpValue = CurrentValue;
	Bind_ShaderResource();
}

void US1MonsterHPBar::ShowBar()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (Anim_FadeIn)
	{
		PlayAnimation(Anim_FadeIn);
	}
}

void US1MonsterHPBar::HideBar()
{
	if (Anim_FadeOut)
	{
		PlayAnimation(Anim_FadeOut);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		OnHideFinished.Broadcast();
	}
}

void US1MonsterHPBar::ForceHide()
{
	StopAllAnimations();
	SetVisibility(ESlateVisibility::Collapsed);
}

void US1MonsterHPBar::NativeConstruct()
{
	Super::NativeConstruct();

	LerpValue = CurrentValue;
	LerpSpeed = 25.f;
	Bind_ShaderResource();

	if (Anim_FadeOut)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &ThisClass::HandleHideAnimationFinished);
		BindToAnimationFinished(Anim_FadeOut, EndEvent);
	}
}

void US1MonsterHPBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(Monster) || Monster->IsDead())
	{
		ForceHide();
		return;
	}

	const US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(Monster->GetAbilitySystemComponent());
	if (ASC == nullptr)
	{
		return;
	}

	const US1AttributeSet* MonsterAttributeSet = Cast<US1AttributeSet>(ASC->GetAttributeSet(US1AttributeSet::StaticClass()));
	if (MonsterAttributeSet == nullptr)
	{
		return;
	}

	SetValue(MonsterAttributeSet->GetHealth(), MonsterAttributeSet->GetMaxHealth());

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

void US1MonsterHPBar::Bind_ShaderResource()
{
	if (Fill_MID == nullptr)
	{
		return;
	}

	Fill_MID->SetScalarParameterValue(FillRatioParameterName, GetFillRatio(CurrentValue));
	Fill_MID->SetScalarParameterValue(LerpRatioParameterName, GetFillRatio(LerpValue));
}

void US1MonsterHPBar::HandleHideAnimationFinished()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OnHideFinished.Broadcast();
}
