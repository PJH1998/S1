// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_ApplyHeal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"

void US1AnimNotify_ApplyHeal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
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

	US1AttributeSet* AttributeSet = const_cast<US1AttributeSet*>(Cast<US1AttributeSet>(ASC->GetAttributeSet(US1AttributeSet::StaticClass())));
	if (nullptr == AttributeSet)
	{
		return;
	}

	const float HealAmount = AttributeSet->GetMaxHealth() * HealPercentOfMaxHealth;
	const float NewHealth = FMath::Min(AttributeSet->GetHealth() + HealAmount, AttributeSet->GetMaxHealth());
	AttributeSet->SetHealth(NewHealth);
}
