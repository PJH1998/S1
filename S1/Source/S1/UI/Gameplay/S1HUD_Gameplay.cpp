// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gameplay/S1HUD_Gameplay.h"
#include "AbilitySystemComponent.h"
#include "Character/Boss/S1BossBase.h"
#include "Kismet/GameplayStatics.h"

#include "UI/Gameplay/Boss/S1BossStatus.h"
#include "UI/Gameplay/S1RespawnPrompt.h"
#include "Animation/WidgetAnimation.h"
#include "Component/S1InteractComponent.h"
#include "Components/Widget.h"
#include "Player/S1PlayerController.h"
#include "Player/S1PlayerState.h"
#include "Tags/S1GameplayTags.h"

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
	BindRespawnEvents();
	SetUpRespawnPrompt();
}

void US1HUD_Gameplay::NativeDestruct()
{
	if (AS1PlayerState* PS = GetOwningPlayerState<AS1PlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			ASC->RegisterGameplayTagEvent(S1StateTags::State_Dead_CanRespawn, EGameplayTagEventType::NewOrRemoved).Remove(CanRespawnTagEventHandle);
		}
	}

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
	// 재호출 대비 — 옛(파괴됐을 수 있는) Pawn의 컴포넌트에 남아있던 구독 해제.
	if (US1InteractComponent* OldInteractComp = BoundInteractComponent.Get())
	{
		OldInteractComp->OnNearestInteractableChanged.RemoveDynamic(this, &ThisClass::HandleNearestInteractableChanged);
	}
	BoundInteractComponent = nullptr;

	APawn* OwningPawn = GetOwningPlayerPawn();
	US1InteractComponent* InteractComp = ::IsValid(OwningPawn) ? OwningPawn->FindComponentByClass<US1InteractComponent>() : nullptr;
	if (InteractComp == nullptr)
	{
		return;
	}

	InteractComp->OnNearestInteractableChanged.AddDynamic(this, &ThisClass::HandleNearestInteractableChanged);
	BoundInteractComponent = InteractComp;

	// 새 Pawn 기준으로 프롬프트 상태도 즉시 갱신(재바인딩 시점엔 아직 오버랩 이벤트가 안 왔을 수 있어 nullptr 취급).
	HandleNearestInteractableChanged(InteractComp->GetNearestInteractable());
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

void US1HUD_Gameplay::HandleNearestInteractableChanged(AActor* NewNearest)
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

void US1HUD_Gameplay::BindRespawnEvents()
{
	AS1PlayerState* PS = GetOwningPlayerState<AS1PlayerState>();
	UAbilitySystemComponent* ASC = ::IsValid(PS) ? PS->GetAbilitySystemComponent() : nullptr;
	if (nullptr == ASC)
	{
		return;
	}

	// AddLooseGameplayTag/RemoveLooseGameplayTag 둘 다 이 이벤트로 잡힘 — 별도 커스텀 이벤트 불필요.
	// 태그 제거 시점(AS1PlayerController::ServerRespawn)이 FadeIn 시작과 사실상 같은 타이밍이라 자연히 맞물림.
	CanRespawnTagEventHandle = ASC->RegisterGameplayTagEvent(S1StateTags::State_Dead_CanRespawn, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ThisClass::HandleCanRespawnTagChanged);
}

void US1HUD_Gameplay::SetUpRespawnPrompt()
{
	if (RespawnPrompt == nullptr)
	{
		return;
	}

	RespawnPrompt->SetVisibility(ESlateVisibility::Collapsed);

	if (Anim_RespawnPrompt_FadeOut)
	{
		FWidgetAnimationDynamicEvent EndEvent;
		EndEvent.BindDynamic(this, &ThisClass::HandleRespawnPromptHideAnimationFinished);
		BindToAnimationFinished(Anim_RespawnPrompt_FadeOut, EndEvent);
	}
}

void US1HUD_Gameplay::ShowRespawnPrompt()
{
	if (RespawnPrompt == nullptr)
	{
		return;
	}

	// InteractPrompt와 달리 클릭 가능해야 하므로 HitTestInvisible이 아니라 Visible.
	RespawnPrompt->SetVisibility(ESlateVisibility::Visible);
	if (Anim_RespawnPrompt_FadeIn)
	{
		PlayAnimation(Anim_RespawnPrompt_FadeIn);
	}

	// 커서 그래픽만 보이는 게 아니라 실제 Input Mode(GameAndUI)+카메라 회전 정지까지 같이 처리돼야
	// 버튼 클릭이 실제로 먹힘(인벤토리/Alt 프리커서와 동일 메커니즘 재사용).
	if (AS1PlayerController* PC = GetOwningPlayer<AS1PlayerController>())
	{
		PC->SetCursorMode(true);
	}
}

void US1HUD_Gameplay::HideRespawnPrompt()
{
	if (RespawnPrompt == nullptr)
	{
		return;
	}

	if (Anim_RespawnPrompt_FadeOut)
	{
		PlayAnimation(Anim_RespawnPrompt_FadeOut);
	}
	else
	{
		RespawnPrompt->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (AS1PlayerController* PC = GetOwningPlayer<AS1PlayerController>())
	{
		PC->SetCursorMode(false);
	}
}

void US1HUD_Gameplay::HandleCanRespawnTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		ShowRespawnPrompt();
	}
	else
	{
		HideRespawnPrompt();
	}
}

void US1HUD_Gameplay::HandleRespawnPromptHideAnimationFinished()
{
	if (RespawnPrompt != nullptr)
	{
		RespawnPrompt->SetVisibility(ESlateVisibility::Collapsed);
	}
}
