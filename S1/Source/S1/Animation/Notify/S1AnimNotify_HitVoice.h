// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "S1AnimNotify_HitVoice.generated.h"

class USoundBase;

// 남녀 공용 Hit 몽타주에 배치 — 재생 주체의 성별(AS1Player::GetGender)에 따라 다른 보이스를 재생
UCLASS()
class S1_API US1AnimNotify_HitVoice : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "HitVoice")
	TObjectPtr<USoundBase> MaleSound;

	UPROPERTY(EditAnywhere, Category = "HitVoice")
	TObjectPtr<USoundBase> FemaleSound;
};
