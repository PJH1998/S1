// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "S1Enums.h"
#include "S1AnimNotify_WeaponDissolve.generated.h"

// 지정한 손의 장착 무기에 Dissolve 애니메이션 재생 — 파티클 연출은 같은 타이밍에 별도 SpawnEffect 계열 Notify로 병행 배치
UCLASS()
class S1_API US1AnimNotify_WeaponDissolve : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Dissolve")
	ES1AttackHand Hand = ES1AttackHand::Main;

	// true: 나타남(0→1), false: 사라짐(1→0)
	UPROPERTY(EditAnywhere, Category = "Dissolve")
	bool bAppear = false;

	UPROPERTY(EditAnywhere, Category = "Dissolve", meta = (ClampMin = "0.0"))
	float Duration = 0.3f;
};
