// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotify_SpawnEffect.generated.h"

UCLASS()
class S1_API US1AnimNotify_SpawnEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	US1AnimNotify_SpawnEffect();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag AssetTag;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName SpawnSocketName;

	UPROPERTY(EditAnywhere, Category = "Effect")
	bool IsApplySocketRotation = true;
};
