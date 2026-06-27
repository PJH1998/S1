// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "S1AbilitySystemComponent.generated.h"

class FLifetimeProperty;

// 서버가 구축한 (입력 AbilityTag → SpecHandle) 한 쌍 — 소유 클라 복제용
USTRUCT()
struct FS1AbilityTagSpec
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	FGameplayAbilitySpecHandle Handle;
};

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// 클라: 복제된 ReplicatedTagSpecs로 TagToSpecHandles 재구성
	// (AddCharacterAbilities는 서버 전용 → LocalPredicted 활성화 시 클라 맵이 비어있는 문제 해결)
	UFUNCTION()
	void OnRep_ReplicatedTagSpecs();

	void RebuildTagToSpecHandlesFromReplicated();

	// 서버가 구축한 매핑을 소유 클라에 복제 — 클라 LocalPredicted 활성화 조회용
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTagSpecs)
	TArray<FS1AbilityTagSpec> ReplicatedTagSpecs;

	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> TagToSpecHandles;
	TMap<FGameplayTag, TArray<FGameplayAbilitySpecHandle>> GroupToSpecHandles;
};
