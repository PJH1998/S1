// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "S1AbilitySystemComponent.generated.h"

UCLASS()
class S1_API US1AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void AddCharacterAbilities(const FGameplayTag& AssetTag);
	virtual void RemoveCharacterAbilities(const FGameplayTag& AssetTag);
	virtual bool ActivateAbility(const FGameplayTag& AbilityTag);

	// 키 업 — 해당 태그의 활성 GA에 OnInputReleased 전달 (비활성이면 무시)
	void ReleaseAbility(const FGameplayTag& AbilityTag);

	// 해당 입력 태그의 GA가 LocalPredicted(예측 활성화)인지 — 입력 라우팅이 클라 직접 활성화 vs 서버 RPC를 선택
	bool IsAbilityPredicted(const FGameplayTag& AbilityTag);

protected:
	// TagToSpecHandles 유지는 엔진 콜백에 맡긴다 — 스펙은 ActivatableAbilities(COND_ReplayOrOwner)로
	// 이미 복제되고, 클라에서도 FGameplayAbilitySpec::PostReplicatedAdd가 OnGiveAbility를 부른다
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	// 전체 재동기(PostReplicatedChange 등) 대비 안전망
	virtual void OnRep_ActivateAbilities() override;

	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> TagToSpecHandles;
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> GroupToSpecHandles;

private:
	void RebuildTagToSpecHandles();
};
