// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffectWithProgress.h"
#include "Character/S1Character.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "NiagaraComponent.h"

void US1AnimNotifyState_SpawnEffectWithProgress::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	const FAnimNotifyEvent* NotifyEvent = EventReference.GetNotify();
	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	if (nullptr == MeshComp || nullptr == NotifyEvent || nullptr == Montage)
	{
		return;
	}

	AS1Character* Character = Cast<AS1Character>(MeshComp->GetOwner());
	if (nullptr == Character)
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (nullptr == AnimInstance)
	{
		return;
	}

	// 파티클 자체 수명이 아니라, 이 Notify가 몽타주 안에서 Begin~End 사이 얼마나 진행됐는지로 진행률 계산
	const float CurrentPosition = AnimInstance->Montage_GetPosition(Montage);
	const float Duration = FMath::Max(NotifyEvent->GetDuration(), KINDA_SMALL_NUMBER);
	const float Progress = FMath::Clamp((CurrentPosition - NotifyEvent->GetTriggerTime()) / Duration, 0.0f, 1.0f);

	const TArray<TWeakObjectPtr<UNiagaraComponent>>* Effects = Character->FindAttachedEffects(GetEffectTag());
	if (nullptr == Effects)
	{
		return;
	}

	for (const TWeakObjectPtr<UNiagaraComponent>& WeakComponent : *Effects)
	{
		if (UNiagaraComponent* Component = WeakComponent.Get())
		{
			Component->SetVariableFloat(ProgressParameterName, Progress);
		}
	}
}
