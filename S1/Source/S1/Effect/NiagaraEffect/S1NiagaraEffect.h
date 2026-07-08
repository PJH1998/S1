// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/S1Effect.h"
#include "S1NiagaraEffect.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;

UCLASS()
class S1_API AS1NiagaraEffect : public AS1Effect
{
	GENERATED_BODY()

public:
	virtual void PlayEffect(UWorld* World, FVector Location, FRotator Rotation, FVector Scale = FVector::OneVector, UNiagaraComponent** OutComponent = nullptr, bool bAutoRelease = true) const override;

	// 소켓에 붙여서 재생 — 붙인 대상이 움직이면 같이 따라감(트레일류). OutComponent로 이후 DetachFromComponent 가능한 컴포넌트를 받음
	// SpawnOffset: 소켓 기준 로컬 오프셋(Location/Rotation/Scale) — Effect Local에 곱해지는 소켓 상대 트랜스폼
	// bAutoRelease=false면 ENCPoolMethod::None — Activate(true)로 반복 재트리거할 컴포넌트는 false로 스폰해야 함(안 그러면 완료 시 풀로 반납되어 재트리거 불가)
	virtual void PlayEffectAttached(USceneComponent* AttachToComponent, FName SocketName, const FTransform& SpawnOffset = FTransform::Identity, UNiagaraComponent** OutComponent = nullptr, bool bAutoRelease = true) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
};
