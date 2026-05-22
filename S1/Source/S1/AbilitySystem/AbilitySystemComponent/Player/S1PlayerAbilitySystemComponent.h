// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "S1PlayerAbilitySystemComponent.generated.h"

UCLASS()
class S1_API US1PlayerAbilitySystemComponent : public US1AbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual bool ActivateAbility(const FGameplayTag& AbilityTag) override;

private:
	// FlushTag — 활성 GA의 GetInputFlushTag()로 결정 (에디터 설정 불필요)
	void QueueAbility(const FGameplayTag& AbilityTag, const FGameplayTag& FlushTag);
	void FlushQueuedAbility();
	void ClearQueue();

	void OnFlushTagChanged(const FGameplayTag Tag, int32 NewCount);

private:
	// 버퍼 유효 시간 (초) — 이 시간 내 조건 미충족 시 자동 클리어
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
