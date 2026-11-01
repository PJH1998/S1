// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/LooseGameplayTag/S1AnimNotifyState_LooseGameplayTag.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "S1LogChannels.h"

void US1AnimNotifyState_LooseGameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SetTag(MeshComp, true);
}

void US1AnimNotifyState_LooseGameplayTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	SetTag(MeshComp, false);
}

void US1AnimNotifyState_LooseGameplayTag::SetTag(USkeletalMeshComponent* MeshComp, bool bAdd)
{
	if (false == Tag.IsValid())
	{
		return;
	}

	// [NotifyThrash] 진단 — 데디 서버에서 NotifyState Begin/End가 매 프레임 재발화하는지 확인용. 실험 후 제거
	if (const AActor* Owner = MeshComp->GetOwner())
	{
		const ENetRole Role = Owner->GetLocalRole();
		LOG(TEXT("[NotifyThrash][%s] %s %s t=%.3f"),
			ROLE_Authority == Role ? TEXT("AUTH") : ROLE_AutonomousProxy == Role ? TEXT("AUTO") : TEXT("SIM"),
			bAdd ? TEXT("Begin") : TEXT("End"),
			*Tag.ToString(),
			Owner->GetWorld()->GetTimeSeconds());
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (nullptr == ASI)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (false == IsValid(ASC))
	{
		return;
	}

	if (bAdd)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}
