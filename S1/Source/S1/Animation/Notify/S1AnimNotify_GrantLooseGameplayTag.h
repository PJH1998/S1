// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotify_GrantLooseGameplayTag.generated.h"

// 몽타주 타임라인에서 서버 권위로 복제 Loose 태그 하나를 부여한다(예: Death 몽타주 Loop 진입 지점에서 State.Dead.CanRespawn).
UCLASS()
class S1_API US1AnimNotify_GrantLooseGameplayTag : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "GameplayTag")
	FGameplayTag TagToGrant;
};
