// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "S1PlayerSet.generated.h"

class FLifetimeProperty;

UCLASS()
class S1_API US1PlayerSet : public US1AttributeSet
{
	GENERATED_BODY()

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void InitAttributeFromTable(const FGameplayTag& AssetTag, const FGameplayTag& TableTag, FName RowName = NAME_None) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void LevelUp();

	// 얼티밋 게이지 증가 — Max로 클램프 후 State.CanUltimate 태그 갱신 (서버 권위 호출 전제)
	void AddUltimateGauge(float Amount);
	// 얼티밋 발동 시 게이지 소모 — 0으로 리셋 + State.CanUltimate 제거
	void ResetUltimateGauge();

public:
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentXP);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxXP);
	ATTRIBUTE_ACCESSORS(ThisClass, Level);
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentUltimateGauge);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxUltimateGauge);

private:
	UFUNCTION()
	void OnRep_CurrentXP(const FGameplayAttributeData& OldCurrentXP);

	UFUNCTION()
	void OnRep_MaxXP(const FGameplayAttributeData& OldMaxXP);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldLevel);

	UFUNCTION()
	void OnRep_CurrentUltimateGauge(const FGameplayAttributeData& OldCurrentUltimateGauge);

	UFUNCTION()
	void OnRep_MaxUltimateGauge(const FGameplayAttributeData& OldMaxUltimateGauge);

	// 현재 게이지 값 기준으로 State.CanUltimate replicated loose tag Add/Remove
	void UpdateCanUltimateTag();

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentXP, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CurrentXP = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxXP, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxXP = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Level = 1.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentUltimateGauge, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData CurrentUltimateGauge = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxUltimateGauge, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxUltimateGauge = 0.f;

	FGameplayTag CachedAssetTag;
	FGameplayTag CachedTableTag;
};
