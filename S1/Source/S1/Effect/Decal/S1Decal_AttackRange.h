// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/Decal/S1Decal.h"
#include "S1Decal_AttackRange.generated.h"

UENUM(BlueprintType)
enum class ES1AttackRangeDecalShape : uint8
{
	Circle,
	Rectangle,
	Cone,
};

USTRUCT(BlueprintType)
struct FS1AttackRangeDecalRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ES1AttackRangeDecalShape Shape = ES1AttackRangeDecalShape::Circle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Angle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Length = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectionDepth = 256.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor(1.f, 0.f, 0.f, 0.65f);
};

UCLASS()
class S1_API AS1Decal_AttackRange : public AS1Decal
{
	GENERATED_BODY()

public:
	void ShowAttackRange(const FS1AttackRangeDecalRequest& InRequest);
	virtual bool UpdateDecal(float DeltaTime) override;
	virtual void HideDecal() override;

private:
	FVector CalculateDecalSize() const;
	void ApplyMaterialParameters(float Progress);

private:
	FS1AttackRangeDecalRequest Request;
	float ElapsedTime = 0.f;
};
