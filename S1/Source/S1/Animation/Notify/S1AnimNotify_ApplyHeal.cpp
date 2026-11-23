// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_ApplyHeal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "AbilitySystem/Effects/S1GameplayEffect_Heal.h"
#include "Tags/S1GameplayTags.h"

US1AnimNotify_ApplyHeal::US1AnimNotify_ApplyHeal()
{
	HealEffectClass = US1GameplayEffect_Heal::StaticClass();
}

void US1AnimNotify_ApplyHeal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp || nullptr == HealEffectClass.Get())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (nullptr == Owner || false == Owner->HasAuthority())
	{
		return;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	UAbilitySystemComponent* ASC = ASI ? ASI->GetAbilitySystemComponent() : nullptr;
	if (nullptr == ASC)
	{
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HealEffectClass, 1.f, ASC->MakeEffectContext());
	if (false == SpecHandle.IsValid())
	{
		return;
	}

	// 회복량만 넘기고 MaxHealth 초과분은 S1AttributeSet::PostGameplayEffectExecute가 클램프한다
	const float HealAmount = ASC->GetNumericAttribute(US1AttributeSet::GetMaxHealthAttribute()) * HealPercentOfMaxHealth;
	SpecHandle.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_Heal, HealAmount);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
