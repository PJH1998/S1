// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "S1AnimNotify_PlayBGMByTag.generated.h"

// 몽타주에 배치 — 태그로 US1SoundData에서 BGM을 조회해 US1SoundManager::PlayBGM으로 전환한다.
// 몽타주는 각 클라에서 로컬 재생되므로 별도 RPC 없이 전 클라에 전파된다 (데디서버는 SoundManager가 no-op).
UCLASS()
class S1_API US1AnimNotify_PlayBGMByTag : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "BGM")
	FGameplayTag BGMTag;

	UPROPERTY(EditAnywhere, Category = "BGM")
	float FadeDuration = 1.5f;

	UPROPERTY(EditAnywhere, Category = "BGM")
	float Volume = 1.f;
};
