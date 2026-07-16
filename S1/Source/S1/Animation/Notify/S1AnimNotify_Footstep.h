// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/EngineTypes.h"
#include "S1AnimNotify_Footstep.generated.h"

// Walk/Sprint AnimSequence에 배치 — 발 소켓에서 아래로 트레이스해 EPhysicalSurface를 얻고
// DataTable(FS1FootstepSoundRow)에서 매칭되는 사운드를 찾아 그 위치에서 재생한다.
UCLASS()
class S1_API US1AnimNotify_Footstep : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	// 좌/우 등 발마다 다른 소켓을 가리키도록 노티파이 인스턴스별로 설정 (하드코딩 금지)
	UPROPERTY(EditAnywhere, Category = "Footstep")
	FName FootSocketName = TEXT("foot_l");

	UPROPERTY(EditAnywhere, Category = "Footstep")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// 소켓 기준 위/아래로 얼마나 트레이스할지 (지면이 소켓보다 살짝 위/아래일 수 있어 소켓 위쪽에서 시작)
	UPROPERTY(EditAnywhere, Category = "Footstep")
	float TraceUpOffset = 10.f;

	UPROPERTY(EditAnywhere, Category = "Footstep")
	float TraceDownDistance = 50.f;
};
