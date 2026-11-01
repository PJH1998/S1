// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/S1GameplayAbility.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"
#include "Data/S1AbilityData.h"
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
		// 입력 태그를 스펙에 실어 복제한다 — OnGiveAbility가 이 태그로 TagToSpecHandles를 채운다
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.AbilityTag);
		FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);

		GroupHandles.Add(Handle);
	}
}

void US1AbilitySystemComponent::RemoveCharacterAbilities(const FGameplayTag& AssetTag)
{
	TArray<FGameplayAbilitySpecHandle>* GroupHandles = GroupToSpecHandles.Find(AssetTag);
	if (nullptr == GroupHandles)
	{
		return;
	}

	// TagToSpecHandles 정리는 ClearAbility → OnRemoveAbility가 핸들 단위로 처리한다
	for (auto& Handle : *GroupHandles)
	{
		ClearAbility(Handle);
	}

	GroupToSpecHandles.Remove(AssetTag);
}

void US1AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		TagToSpecHandles.FindOrAdd(Tag).AddUnique(AbilitySpec.Handle);
	}
}

void US1AbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	// 핸들 단위로만 지운다 — 같은 AbilityTag를 다른 AbilityData 그룹이 공유할 때
	// 배열을 통째로 비우면 아직 유효한 핸들까지 날아간다(기존 Remove(Tag) 함정)
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (TArray<FGameplayAbilitySpecHandle>* Handles = TagToSpecHandles.Find(Tag))
		{
			Handles->RemoveSingle(AbilitySpec.Handle);
			if (Handles->Num() <= 0)
			{
				TagToSpecHandles.Remove(Tag);
			}
		}
	}
}

void US1AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	RebuildTagToSpecHandles();
}

void US1AbilitySystemComponent::RebuildTagToSpecHandles()
{
	TagToSpecHandles.Reset();

	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : Spec.GetDynamicSpecSourceTags())
		{
			TagToSpecHandles.FindOrAdd(Tag).AddUnique(Spec.Handle);
		}
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

	// 재입력(콤보 진행)은 새 활성화가 아니라 "이미 활성 중인 GA의 재활성"이다.
	// 활성 GA의 Blocked 태그(예: State.Used.Air.*)는 새 활성화만 막고 재활성은 안 막으므로,
	// DoesAbilitySatisfyTagRequirements로 판정하면 공중 재입력 때 모든 변형이 탈락→false→ServerOnly 경로로 새어
	// 소유 클라의 예측 재활성이 깨진다(서버만 콤보 진행, 클라는 1타에서 멈춤). → 활성 GA가 있으면 그 정책으로 먼저 판정.
	for (auto& Handle : *Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (nullptr == Spec || nullptr == Spec->Ability || false == Spec->IsActive())
		{
			continue;
		}

		return EGameplayAbilityNetExecutionPolicy::LocalPredicted == Spec->Ability->GetNetExecutionPolicy();
	}

	// 활성 GA 없음 = 새 활성화 — 같은 AbilityTag 아래 정책이 다른 변형이 섞일 수 있으니(예: Skill04 Ground/Air는
	// LocalPredicted, 게이지 Max 시 같은 태그의 Ultimate는 ServerOnly) 현재 Required/Blocked 태그를 실제로
	// 만족하는 후보만 봐야 "지금 발동될 GA"의 정책을 올바르게 판정할 수 있음
	for (auto& Handle : *Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (nullptr == Spec || nullptr == Spec->Ability)
		{
			continue;
		}

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
