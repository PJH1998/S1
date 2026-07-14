// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/S1HUD_Gameplay.h"
#include "Character/Boss/S1BossBase.h"
#include "Kismet/GameplayStatics.h"

#include "UI/Gameplay/Boss/S1BossStatus.h"
#include "Animation/WidgetAnimation.h"
#include "Component/S1InteractComponent.h"
#include "Components/Widget.h"
#include "Interaction/Gimmick/S1PuzzleButton_Gimmick.h"

US1HUD_Gameplay::US1HUD_Gameplay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1HUD_Gameplay::NativeConstruct()
{
	Super::NativeConstruct();

	BindBossEvents();
	SetUpBossStatus();
	BindInteractEvents();
	SetUpInteractPrompt();
}

void US1HUD_Gameplay::NativeDestruct()
{
	if (BossStatus != nullptr)
	{
		BossStatus->SetBoss(nullptr);
	}

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

	if (US1InteractComponent* InteractComp = BoundInteractComponent.Get())
	{
		InteractComp->OnNearestInteractableChanged.RemoveDynamic(this, &ThisClass::HandleNearestInteractableChanged);
	}
	BoundInteractComponent = nullptr;

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
	if (BossStatus == nullptr)
	{
		return;
	}

	BossStatus->SetBoss(nullptr);
	BossStatus->SetVisibility(ESlateVisibility::Collapsed);

	if (Anim_BossStatus_FadeOut)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &ThisClass::HandleHideAnimationFinished);
		BindToAnimationFinished(Anim_BossStatus_FadeOut, EndEvent);
	}
}

void US1HUD_Gameplay::ShowBossUI(AS1BossBase* InBoss)
{
	if (BossStatus == nullptr)
	{
		return;
	}

	CurrentBoss = InBoss;
	
	BossStatus->SetBoss(InBoss);
	BossStatus->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (Anim_BossStatus_FadeIn)
	{
		PlayAnimation(Anim_BossStatus_FadeIn);
	}
}

void US1HUD_Gameplay::HideBossUI(AS1BossBase* InBoss)
{
	if (BossStatus == nullptr)
	{
		return;
	}

	if (CurrentBoss != nullptr && CurrentBoss != InBoss)
	{
		return;
	}

	CurrentBoss = nullptr;
	BossStatus->SetBoss(nullptr);
	if (Anim_BossStatus_FadeOut)
	{
		PlayAnimation(Anim_BossStatus_FadeOut);
	}
	else
	{
		BossStatus->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void US1HUD_Gameplay::HandleBossHasTargetChanged(AS1Monster* InMonster, bool bInHasTarget)
{
	AS1BossBase* Boss = Cast<AS1BossBase>(InMonster);
	if (Boss == nullptr)
	{
		return;
	}

	if (bInHasTarget)
	{
		ShowBossUI(Boss);
	}
	else
	{
		HideBossUI(Boss);
	}
}

void US1HUD_Gameplay::HandleHideAnimationFinished()
{
	if (BossStatus != nullptr)
	{
		BossStatus->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void US1HUD_Gameplay::BindInteractEvents()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	US1InteractComponent* InteractComp = ::IsValid(OwningPawn) ? OwningPawn->FindComponentByClass<US1InteractComponent>() : nullptr;
	if (InteractComp == nullptr)
	{
		return;
	}

	InteractComp->OnNearestInteractableChanged.AddDynamic(this, &ThisClass::HandleNearestInteractableChanged);
	BoundInteractComponent = InteractComp;
}

void US1HUD_Gameplay::SetUpInteractPrompt()
{
	if (InteractPrompt == nullptr)
	{
		return;
	}

	InteractPrompt->SetVisibility(ESlateVisibility::Collapsed);

	if (Anim_InteractPrompt_FadeOut)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &ThisClass::HandleInteractPromptHideAnimationFinished);
		BindToAnimationFinished(Anim_InteractPrompt_FadeOut, EndEvent);
	}
}

void US1HUD_Gameplay::ShowInteractPrompt()
{
	if (InteractPrompt == nullptr)
	{
		return;
	}

	InteractPrompt->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (Anim_InteractPrompt_FadeIn)
	{
		PlayAnimation(Anim_InteractPrompt_FadeIn);
	}
}

void US1HUD_Gameplay::HideInteractPrompt()
{
	if (InteractPrompt == nullptr)
	{
		return;
	}

	if (Anim_InteractPrompt_FadeOut)
	{
		PlayAnimation(Anim_InteractPrompt_FadeOut);
	}
	else
	{
		InteractPrompt->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void US1HUD_Gameplay::HandleNearestInteractableChanged(AS1PuzzleButton_Gimmick* NewNearest)
{
	if (NewNearest != nullptr)
	{
		ShowInteractPrompt();
	}
	else
	{
		HideInteractPrompt();
	}
}

void US1HUD_Gameplay::HandleInteractPromptHideAnimationFinished()
{
	if (InteractPrompt != nullptr)
	{
		InteractPrompt->SetVisibility(ESlateVisibility::Collapsed);
	}
}
