// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Effect/Decal/S1Decal_AttackRange.h"
#include "S1AnimNotifyState_AttackRange.generated.h"

class AS1Decal_AttackRange;
class UGameplayEffect;

UCLASS()
class S1_API US1AnimNotifyState_AttackRange : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	FS1AttackRangeDecalRequest MakeRequest(USkeletalMeshComponent* MeshComp, float TotalDuration) const;
	void ApplyRangeDamage(USkeletalMeshComponent* MeshComp);
	bool IsInsideCone(const FS1AttackRangeDecalRequest& Request, const FVector& TargetLocation) const;
	bool HasHitActor(USkeletalMeshComponent* MeshComp, AActor* TargetActor) const;
	void AddHitActor(USkeletalMeshComponent* MeshComp, AActor* TargetActor);

private:
	UPROPERTY(EditAnywhere, Category = "AttackRange|Shape")
	ES1AttackRangeDecalShape Shape = ES1AttackRangeDecalShape::Cone;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Circle,Cone")
	float Radius = 300.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Cone")
	float Angle = 90.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Rectangle")
	float Width = 100.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Rectangle")
	float Length = 300.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Projection")
	float ProjectionDepth = 32.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Projection")
	bool bProjectToGround = true;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Projection", meta = (EditCondition = "bProjectToGround"))
	float GroundTraceHeight = 500.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Projection", meta = (EditCondition = "bProjectToGround"))
	float GroundTraceDistance = 3000.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Transform")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Transform")
	float YawOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Visual")
	FLinearColor Color = FLinearColor(1.f, 0.f, 0.f, 0.65f);

	UPROPERTY(EditAnywhere, Category = "AttackRange|Damage")
	bool bApplyDamage = false;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Damage", meta = (EditCondition = "bApplyDamage"))
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Damage", meta = (EditCondition = "bApplyDamage"))
	float DamageRatio = 1.f;

	UPROPERTY(EditAnywhere, Category = "AttackRange|Damage", meta = (EditCondition = "bApplyDamage"))
	float DamageHeight = 200.f;

	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, TWeakObjectPtr<AS1Decal_AttackRange>> ActiveDecals;

	TMap<TWeakObjectPtr<USkeletalMeshComponent>, TArray<TWeakObjectPtr<AActor>>> HitActors;
};
