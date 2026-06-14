// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "S1PlayerAbilitySystemComponent.generated.h"

// 통합 쿨다운 태그 1개에 대응하는 Ground/Air 변형 쿨다운 태그 묶음
USTRUCT()
struct FS1CooldownVariantSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GroundCooldownTag;

	// 비어있으면 공중에서도 Ground 쿨다운 반환 (Air 변형 없는 스킬)
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AirCooldownTag;
};

UCLASS()
class S1_API US1PlayerAbilitySystemComponent : public US1AbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual bool ActivateAbility(const FGameplayTag& AbilityTag) override;

	// UI용 쿨타임 조회 — 통합 태그를 받아 현재 상태(State.Air)에 맞는 변형 쿨다운의 남은 시간/총 시간 반환
	// return: 쿨타임 진행 중이면 true (false면 사용 가능 상태, Out 값은 0)
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	bool GetSkillCooldown(const FGameplayTag& UnifiedCooldownTag, float& OutRemainingTime, float& OutDuration) const;

private:
	// FlushTag — 활성 GA의 GetInputFlushTag()로 결정 (에디터 설정 불필요)
	void QueueAbility(const FGameplayTag& AbilityTag, const FGameplayTag& FlushTag);
	void FlushQueuedAbility();
	void ClearQueue();

	void OnFlushTagChanged(const FGameplayTag Tag, int32 NewCount);

private:
	// 키 입력 버퍼 유효 시간
	UPROPERTY(EditDefaultsOnly, Category = "InputBuffer")
	float InputBufferDuration = 0.1f;

	// 키 입력에 따른 실행할 AbilityTag 보관
	FGameplayTag QueuedAbilityTag;

	// QueueAbility 시 활성 GA로부터 받은 FlushTag 보관 ( Delegate 트리거 )
	FGameplayTag QueuedFlushTag;  
	
	// 키 입력 보관 시간
	FTimerHandle QueueClearTimer;

	// FlushTag를 통해 실행될 이벤트 핸들
	FDelegateHandle TagEventHandle;
};
