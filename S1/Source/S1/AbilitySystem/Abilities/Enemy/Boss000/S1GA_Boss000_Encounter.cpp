// Fill out your copyright notice in the Description page of Project Settings.


#include "S1GA_Boss000_Encounter.h"

#include "Animation/AnimInstance.h"
#include "Character/S1Monster.h"
#include "Data/S1AnimData.h"
#include "System/S1GimmickManager.h"
#include "Tags/Gimmick/S1GameplayTags_Gimmick.h"

US1GA_Boss000_Encounter::US1GA_Boss000_Encounter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_Encounter::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const FS1MontageData* MontageSet = GetMontage();
	if (nullptr == MontageSet || nullptr == MontageSet->Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AS1Monster* Monster = Cast<AS1Monster>(ActorInfo->AvatarActor);
	if (nullptr == Monster)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 인카운터(보스전 시작) 시 부서진 기둥들을 원상 복구한다(서버 전용).
	if (Monster->HasAuthority())
	{
		if (US1GimmickManager* GimmickManager = Monster->GetWorld()->GetSubsystem<US1GimmickManager>())
		{
			GimmickManager->ResetGimmicks(S1GimmickTags::Gimmick_Boss000_Pillar);
		}
	}

	UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
	if (nullptr == AnimInstance)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Animation
	if (PlayAttackMontage(MontageSet->Montage) == false)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void US1GA_Boss000_Encounter::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}