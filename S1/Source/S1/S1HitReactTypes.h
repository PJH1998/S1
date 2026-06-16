// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1Enums.h"
#include "S1HitReactTypes.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FS1HitReactMontages
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Ground|Weak")
	TMap<ES1Direction, TObjectPtr<UAnimMontage>> WeakHits;

	UPROPERTY(EditAnywhere, Category = "Ground|Strong")
	TMap<ES1Direction, TObjectPtr<UAnimMontage>> StrongHits;

	UPROPERTY(EditAnywhere, Category = "Launch")
	TObjectPtr<UAnimMontage> LaunchMontage;

	UPROPERTY(EditAnywhere, Category = "Launch")
	FName LaunchStartSection = TEXT("Start");

	UPROPERTY(EditAnywhere, Category = "Launch")
	FName LaunchTopSection = TEXT("Top");

	UPROPERTY(EditAnywhere, Category = "Launch")
	FName LaunchLoopSection = TEXT("Loop");

	UPROPERTY(EditAnywhere, Category = "Launch")
	FName LaunchEndSection = TEXT("End");

	UPROPERTY(EditAnywhere, Category = "Launch")
	TObjectPtr<UAnimMontage> GetUp;
};
