// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"
#include "Data/S1AbilityData.h"
#include "Net/UnrealNetwork.h"

void US1AbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 소유 클라만 — LocalPredicted 활성화 조회에만 필요
	DOREPLIFETIME_CONDITION(US1AbilitySystemComponent, ReplicatedTagSpecs, COND_OwnerOnly);
}

void US1AbilitySystemComponent::AddCharacterAbilities(const FGameplayTag& AssetTag)
{
	// 이미 부여된 그룹이면 재부여 안 함 — 안 막으면 리스폰처럼 같은 ASC에 PossessedBy가 다시 호출되는
	// 경우(ASC는 PlayerState 소속이라 Pawn과 달리 리스폰에도 안 사라짐) 같은 어빌리티가 중복 부여되어
	// GameplayEvent 트리거 시 여러 인스턴스가 동시에 활성화·서로 CancelAllAbilities로 취소하는 문제 발생.
	if (GroupToSpecHandles.Contains(AssetTag))
	{
		return;
	}

	US1AbilityData* AbilityData = US1AssetManager::GetAssetByTag<US1AbilityData>(AssetTag);

	TArray<FGameplayAbilitySpecHandle>& GroupHandles = GroupToSpecHandles.FindOrAdd(AssetTag);

	for (auto& AbilitySet : AbilityData->Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilitySet.Abilitiy, AbilitySet.AbilityLevel);
		FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

		GroupHandles.Add(Handle);
		TagToSpecHandles.FindOrAdd(AbilitySet.AbilityTag).Add(Handle);

		// 소유 클라 복제용 매핑 기록
		FS1AbilityTagSpec TagSpec;
		TagSpec.Tag = AbilitySet.AbilityTag;
		TagSpec.Handle = Handle;
		ReplicatedTagSpecs.Add(TagSpec);
	}
}

void US1AbilitySystemComponent::RemoveCharacterAbilities(const FGameplayTag& AssetTag)
{
	TArray<FGameplayAbilitySpecHandle>* GroupHandles = GroupToSpecHandles.Find(AssetTag);
	if (nullptr == GroupHandles)
	{
		return;
	}

	// 복제 매핑에서 이 그룹의 핸들 제거
	ReplicatedTagSpecs.RemoveAll([GroupHandles](const FS1AbilityTagSpec& Entry)
	{
		return GroupHandles->Contains(Entry.Handle);
	});

	for (auto& Handle : *GroupHandles)
	{
		ClearAbility(Handle);
	}

	US1AbilityData* AbilityData = US1AssetManager::GetAssetByTag<US1AbilityData>(AssetTag);
	for (auto& AbilitySet : AbilityData->Abilities)
	{
		TagToSpecHandles.Remove(AbilitySet.AbilityTag);
	}

	GroupToSpecHandles.Remove(AssetTag);
}

void US1AbilitySystemComponent::OnRep_ReplicatedTagSpecs()
{
	// 클라: 서버가 복제한 매핑으로 TagToSpecHandles 재구성
	RebuildTagToSpecHandlesFromReplicated();
}

void US1AbilitySystemComponent::RebuildTagToSpecHandlesFromReplicated()
{
	TagToSpecHandles.Reset();

	for (const FS1AbilityTagSpec& Entry : ReplicatedTagSpecs)
	{
		TagToSpecHandles.FindOrAdd(Entry.Tag).Add(Entry.Handle);
	}
}

void US1AbilitySystemComponent::ReleaseAbility(const FGameplayTag& AbilityTag)
{
	TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == Handles)
	{
		return;
	}

	for (auto& Handle : *Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (nullptr == Spec || false == Spec->IsActive())
		{
			continue;
		}

		if (US1GameplayAbility* GA = Cast<US1GameplayAbility>(Spec->GetPrimaryInstance()))
		{
			GA->OnInputReleased();
		}
		return;
	}
}

bool US1AbilitySystemComponent::IsAbilityPredicted(const FGameplayTag& AbilityTag)
{
	TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == Handles)
	{
		return false;
	}

	for (auto& Handle : *Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (nullptr == Spec || nullptr == Spec->Ability)
		{
			continue;
		}

		// 같은 AbilityTag 아래 정책이 다른 변형이 섞일 수 있음(예: Skill04 Ground/Air는 LocalPredicted,
		// 게이지 Max 시 같은 태그로 묶이는 Ultimate는 ServerOnly) — 현재 Required/Blocked 태그를
		// 실제로 만족하는 후보만 봐야 "지금 발동될 GA"의 정책을 올바르게 판정할 수 있음
		if (false == Spec->Ability->DoesAbilitySatisfyTagRequirements(*this))
		{
			continue;
		}

		if (EGameplayAbilityNetExecutionPolicy::LocalPredicted == Spec->Ability->GetNetExecutionPolicy())
		{
			return true;
		}
	}

	return false;
}

bool US1AbilitySystemComponent::ActivateAbility(const FGameplayTag& AbilityTag)
{
	TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(AbilityTag);
	if (nullptr == Handles)
	{
		return false;
	}

	bool bActivated = false;
	for (auto& Handle : *Handles)
	{
		bActivated |= TryActivateAbility(Handle);
	}

	return bActivated;
}
