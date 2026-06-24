#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/S1GameplayAbility_Attack.h"
#include "S1GameplayAbility_Ultimate.generated.h"

class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;
class AS1Monster;
class ACineCameraActor;

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
	void PlayCutscene();

	// 시퀀스가 스폰한 CineCamera를 찾아 진입 블렌드 (스폰 평가 지연 대비 1회 재시도)
	void TryBlendToCineCamera();
	ACineCameraActor* FindSequenceCineCamera() const;

	void FreezeMonsters(UWorld* World);
	void UnfreezeMonsters();

	// 시네캠이 살아있는 동안(종료 직전) 게임플레이로 복귀 블렌드 시작
	void StartExitBlend();

	UFUNCTION() void OnCutsceneFinished();
	void OnBlendOutComplete();

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
	UPROPERTY() TObjectPtr<ULevelSequencePlayer> SequencePlayer;
	UPROPERTY() TObjectPtr<ALevelSequenceActor>  SequenceActor;

	// 시퀀스가 스폰한 카메라 (Spawnable, When Finished=Keep State) — 종료 시 직접 파괴
	UPROPERTY() TObjectPtr<ACineCameraActor>     CineCamera;

	// 컷씬 시작~복귀 블렌드 완료까지 true — EndAbility defer 게이트
	bool bCutsceneActive = false;
	// 카메라 진입 블렌드 재시도 1회 가드
	bool bCameraBlendRetried = false;
	// 복귀 블렌드 중복 시작 가드 (타이머 + OnFinished 폴백)
	bool bExitBlendStarted = false;

	FTimerHandle ExitBlendTimerHandle;
	FTimerHandle BlendOutTimerHandle;

	TArray<TWeakObjectPtr<AS1Monster>> FrozenMonsters;
};
