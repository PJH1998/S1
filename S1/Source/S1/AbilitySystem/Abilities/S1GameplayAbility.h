// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "S1GameplayAbility.generated.h"

class US1AnimInstance;

UCLASS()
class S1_API US1GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	US1GameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 동일 GA 입력이 들어왔을 때 기능 구현 ( 필요시 Overrde )
	virtual bool		OnInputReactivated() { return false; }

	// GA 활성 중 키 업 수신 ( 필요시 Override ) — HoldLoop 등 홀드 입력 GA용
	virtual void		OnInputReleased() {}

	// 다른 GA 입력이 들어왔을 때 기능 구현 ( 필요시 Overrde ) / FGameplayTagContainer에 전환 하려는 Ability의 Tag를 모은 후 전달
	virtual bool		OnCrossInput(const FGameplayTagContainer& TargetAbilityTags) { return false; }

	// 입력 큐 실행 트리거 Tag ( 필요시 Override ) / Return Tag가 ASC에 추가될 때 보관중인 입력에 대한 GA 실행 ( Return Tag를 Event로 사용 )
	virtual FGameplayTag GetInputFlushTag() const { return FGameplayTag(); }

protected:
	virtual bool		CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void		ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void		EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	US1AnimInstance*	GetAnimInstance() const;

	// GravityScale 변경 및 이전 GravityScale 보관
	void				SetGravityScale(float Scale);
	// 이전 GravityScale로 복구
	void				ResetGravityScale();

private:
	float	PrevGravityScale = 1.f;
	bool	bGravityScaleSaved = false;
};
