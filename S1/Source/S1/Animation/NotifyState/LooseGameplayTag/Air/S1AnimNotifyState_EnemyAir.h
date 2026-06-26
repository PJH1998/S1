// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "S1AnimNotifyState_EnemyAir.generated.h"

/**
 * 몬스터 전용 점프 발사 NotifyState.
 * 서버에서 ApexHeight로 발사 속도를 역산해 LaunchCharacter로 띄우고,
 * AS1Monster::EnterAirborneState로 정점 감지를 무장한다.
 */
UCLASS()
class S1_API US1AnimNotifyState_EnemyAir : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

private:
	/** 시작점 대비 목표 정점 높이. 발사 속도는 중력에서 역산(v=sqrt(2*|g|*h)). */
	UPROPERTY(EditAnywhere, Category = "Air")
	float ApexHeight = 500.f;
};