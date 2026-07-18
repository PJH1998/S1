// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "S1AnimNotify_DespawnHealItem.generated.h"

// US1AnimNotify_SpawnHealItem으로 부착한 힐 아이템 소품을 Dissolve로 제거
UCLASS()
class S1_API US1AnimNotify_DespawnHealItem : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "HealItem", meta = (ClampMin = "0.0"))
	float DissolveDuration = 0.3f;
};
