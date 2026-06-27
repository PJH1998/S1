#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_Ultimate.generated.h"

class ULevelSequence;
class AS1Monster;

UCLASS()
class S1_API US1GameplayAbility_Ultimate : public US1GameplayAbility_Attack
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Ultimate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 몬스터 프리즈는 서버 권위 (전체 몬스터)
	void FreezeMonsters(UWorld* World);
	void UnfreezeMonsters();

	// 시퀀스 길이만큼 프리즈 유지 후 종료 (컷씬 비주얼은 시전 클라가 처리)
	void  OnServerCutsceneComplete();
	float ComputeSequenceDurationSeconds() const;

	void DoEndAbility(const FGameplayAbilitySpecHandle& Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Cutscene")
	TObjectPtr<ULevelSequence> CutsceneSequence;

	// 시퀀스 내 플레이어 바인딩 태그 (Possessable, 런타임 SetBindingByTag로 실제 폰 주입)
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Cutscene")
	FName PlayerBindingTag = TEXT("PlayerAnchor");

	// 게임플레이 → 컷씬 카메라 진입 블렌드 시간
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Cutscene")
	float EnterBlendTime = 0.3f;

	// 컷씬 카메라 → 게임플레이 복귀 블렌드 시간
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Cutscene")
	float ExitBlendTime = 0.5f;

private:
	// 컷씬 진행 중 true — 몽타주가 먼저 끝나도 EndAbility를 defer (서버 타이머가 최종 종료)
	bool bCutsceneActive = false;

	// 서버 프리즈 유지 타이머
	FTimerHandle CutsceneServerTimer;

	TArray<TWeakObjectPtr<AS1Monster>> FrozenMonsters;
};
