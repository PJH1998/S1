// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S1Effect.generated.h"

class UNiagaraComponent;

UCLASS()
class S1_API AS1Effect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AS1Effect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// bAutoRelease=false면 ENCPoolMethod::None으로 스폰 — 재사용/재트리거(Activate(true))할 컴포넌트는 시스템 완료 시 풀로 자동 반납되면 안 되므로 false 필요
	virtual void PlayEffect(UWorld* World, FVector Location, FRotator Rotation, FVector Scale = FVector::OneVector, UNiagaraComponent** OutComponent = nullptr, bool bAutoRelease = true) const {}
};
