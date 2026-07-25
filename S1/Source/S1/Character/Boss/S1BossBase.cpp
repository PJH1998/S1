// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Boss/S1BossBase.h"

#include "AbilitySystem/Attributes/S1BossSet.h"
#include "AI/S1BossAIController.h"
#include "Engine/LocalPlayer.h"
#include "System/S1CombatFeedbackSubsystem.h"
#include "System/S1SoundManager.h"
#include "System/S1UIManager.h"
#include "Tags/S1GameplayTags.h"

ES1EnemyTier AS1BossBase::GetLockOnTier_Implementation()
{
	return ES1EnemyTier::Boss;
}

AS1BossBase::AS1BossBase()
	: Super()
{
	// 보스는 페이즈 전환 블랙보드를 다루는 전용 컨트롤러를 사용한다.
	AIControllerClass = AS1BossAIController::StaticClass();
}

void AS1BossBase::RequestPhaseTransition()
{
	if (false == HasAuthority() || bPhaseTransitionTriggered)
	{
		return;
	}
	bPhaseTransitionTriggered = true;

	if (AS1BossAIController* BossAIController = Cast<AS1BossAIController>(GetController()))
	{
		BossAIController->SetPhaseTransitionPending(true);
	}
}

void AS1BossBase::RequestLowHpBgm()
{
	if (false == HasAuthority() || bLowHpBgmTriggered)
	{
		return;
	}
	bLowHpBgmTriggered = true;

	MulticastPlayLowHpBgm();
}

US1BossSet* AS1BossBase::GetS1BossSet() const
{
	return Cast<US1BossSet>(AttributeSet);
}

const FName& AS1BossBase::GetBossName() const
{
	return BossName;
}

void AS1BossBase::MulticastNotifyGuardBlocked_Implementation(const FVector& InBlockLocation)
{
	if (UWorld* World = GetWorld())
	{
		if (US1CombatFeedbackSubsystem* CombatFeedback = World->GetSubsystem<US1CombatFeedbackSubsystem>())
		{
			CombatFeedback->ShowBlockNumber(InBlockLocation);
		}

		// 가드 차단음은 US1BossSet가 MulticastPlayHitSound(HitType::Guard)로 무기별 재생 — 여기선 블록 넘버만.
	}
}

void AS1BossBase::MulticastPlayLowHpBgm_Implementation()
{
	if (UWorld* World = GetWorld())
	{
		if (US1SoundManager* SoundManager = World->GetSubsystem<US1SoundManager>())
		{
			SoundManager->PlayBGMByTag(S1SoundTags::Sound_BGM_Boss000_Battle_Fiver, 1.5f);
		}
	}
}

void AS1BossBase::OnDeathPresentationComplete()
{
	Super::OnDeathPresentationComplete();

	if (UWorld* World = GetWorld())
	{
		if (US1SoundManager* SoundManager = World->GetSubsystem<US1SoundManager>())
		{
			SoundManager->PlayBGMByTag(S1SoundTags::Sound_BGM_Boss000_Field, 1.5f);
		}

		// US1UIManager는 ULocalPlayerSubsystem이라 World->GetSubsystem으로 접근 불가 — 로컬 플레이어 경유.
		// 데디서버는 로컬 플레이어가 없어 자연히 no-op.
		APlayerController* PC = World->GetFirstPlayerController();
		ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
		if (US1UIManager* UIManager = LocalPlayer ? LocalPlayer->GetSubsystem<US1UIManager>() : nullptr)
		{
			UIManager->ShowAnnounce(S1UIResourceTags::UI_Icon_Announce_BossCleared);
		}
	}
}
