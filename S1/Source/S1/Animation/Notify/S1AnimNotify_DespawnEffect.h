// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotify_DespawnEffect.generated.h"

// US1AnimNotify_SpawnPersistentEffect와 EffectTag로 짝을 이뤄 사용 — 같은 몽타주가 아니어도(Branch로 점프한
// 다른 몽타주 시작 지점 등) 배치 가능. 부착 여부는 Character가 컴포넌트 상태로 자동 판단하므로 여기서 알 필요 없음
UCLASS()
class S1_API US1AnimNotify_DespawnEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Effect")
	FGameplayTag EffectTag;

	// 0 초과면 즉시 정리하지 않고 이 시간(초) 뒤에 강제 Deactivate — 페이드아웃 등 여유를 두고 싶을 때
	UPROPERTY(EditAnywhere, Category = "Effect")
	float LifetimeAfterEnd = 0.f;
};
