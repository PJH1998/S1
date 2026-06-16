// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "S1GameplayAbility_Ultimate.generated.h"

class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;
class AS1Monster;

// 궁극기 컷씬 — 몬스터 개별 정지(CustomTimeDilation) + Level Sequence(카메라) 동시 재생, 종료 시 정리
UCLASS()
class S1_API US1GameplayAbility_Ultimate : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Ultimate(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void PlayCutscene();

	// World 전체가 아닌 몬스터 액터만 개별로 정지/복원 — WorldTimeDilation을 건드리지 않아 시퀀스/플레이어 몽타주는 정상 속도로 재생됨
	void FreezeMonsters(UWorld* World);
	void UnfreezeMonsters();

	UFUNCTION()
	void OnCutsceneFinished();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Cutscene")
	TObjectPtr<ULevelSequence> CutsceneSequence;

	// Sequencer에서 플레이어 Spawnable 바인딩에 부여한 Tag (우클릭 → Tags → New Tag)
	UPROPERTY(EditDefaultsOnly, Category = "Ultimate|Cutscene")
	FName PlayerBindingTag = TEXT("PlayerAnchor");

private:
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;

	UPROPERTY()
	TObjectPtr<ALevelSequenceActor> SequenceActor;

	TArray<TWeakObjectPtr<AS1Monster>> FrozenMonsters;
};
