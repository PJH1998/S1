// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/S1Effect.h"
#include "S1NiagaraEffect.generated.h"

class UNiagaraSystem;

UCLASS()
class S1_API AS1NiagaraEffect : public AS1Effect
{
	GENERATED_BODY()

public:
	virtual void PlayEffect(UWorld* World, FVector Location, FRotator Rotation) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
};
