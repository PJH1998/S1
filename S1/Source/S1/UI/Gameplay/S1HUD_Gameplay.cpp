// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/S1HUD_Gameplay.h"
#include "Character/Boss/S1BossBase.h"
#include "Kismet/GameplayStatics.h"

#include "UI/Gameplay/Boss/S1BossStatus.h"
#include "Animation/WidgetAnimation.h"

US1HUD_Gameplay::US1HUD_Gameplay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1HUD_Gameplay::NativeConstruct()
{
	Super::NativeConstruct();

	BindBossEvents();
	SetUpBossStatus();
}

void US1HUD_Gameplay::NativeDestruct()
{
	for (AS1BossBase* Boss : BoundBosses)
	{
		if (Boss == nullptr)
		{
			continue;
		}

		Boss->OnHasTargetChanged.RemoveDynamic(this, &ThisClass::HandleBossHasTargetChanged);
	}

	BoundBosses.Reset();
	CurrentBoss = nullptr;

	Super::NativeDestruct();
}

void US1HUD_Gameplay::BindBossEvents()
{
	TArray<AActor*> BossActors;
	UGameplayStatics::GetAllActorsOfClass(this, AS1BossBase::StaticClass(), BossActors);

	for (AActor* BossActor : BossActors)
	{
		AS1BossBase* Boss = Cast<AS1BossBase>(BossActor);
		if (Boss == nullptr)
		{
			continue;
		}

		Boss->OnHasTargetChanged.RemoveDynamic(this, &ThisClass::HandleBossHasTargetChanged);
		Boss->OnHasTargetChanged.AddDynamic(this, &ThisClass::HandleBossHasTargetChanged);
		BoundBosses.Add(Boss);
	}
}

void US1HUD_Gameplay::SetUpBossStatus()
{
	BossStatus->SetVisibility(ESlateVisibility::Collapsed);

	// TODO
	//if (Anim_BossStatus_FadeIn)
	//{
	//	FWidgetAnimationDynamicEvent EndEvent;
	//	EndEvent.BindDynamic(this, &ThisClass::HandleHideAnimationFinished);
	//	BindToAnimationFinished(Anim_BossStatus_FadeIn, EndEvent);
	//}
}

void US1HUD_Gameplay::ShowBossUI(AS1BossBase* InBoss)
{
	CurrentBoss = InBoss;
	
	BossStatus->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (Anim_BossStatus_FadeIn)
	{
		PlayAnimation(Anim_BossStatus_FadeIn);
	}
}

void US1HUD_Gameplay::HideBossUI(AS1BossBase* InBoss)
{
	if (CurrentBoss != nullptr && CurrentBoss != InBoss)
	{
		return;
	}

	CurrentBoss = nullptr;
	BossStatus->SetVisibility(ESlateVisibility::Collapsed);
}

void US1HUD_Gameplay::HandleBossHasTargetChanged(AS1BossBase* InBoss, bool bInHasTarget)
{
	if (bInHasTarget)
	{
		ShowBossUI(InBoss);
	}
	else
	{
		HideBossUI(InBoss);
	}
}

void US1HUD_Gameplay::HandleHideAnimationFinished()
{
	BossStatus->SetVisibility(ESlateVisibility::Collapsed);
}
