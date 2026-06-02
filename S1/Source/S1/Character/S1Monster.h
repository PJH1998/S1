// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "Interface/S1PoolingInterface.h"
#include "S1Monster.generated.h"
class UBehaviorTree;
class UAnimMontage;
class US1DeathPresentationComponent;
UCLASS()
class S1_API AS1Monster : public AS1Character, public IS1PoolingInterface
{
	GENERATED_BODY()
public:
	AS1Monster();
protected:
	virtual void BeginPlay() override;
public:
	UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	void PlayAnimation(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	/** HP 0 시 1회: bIsDead, 어빌리티 취소, BB bIsDead (BT는 유지). */
	virtual void NotifyDeath();
	/** 데스 포즈 고정. MontageToStop 지정 시 해당 AM만 정지(중복 안전). */
	void FreezeDeathPose(UAnimMontage* MontageToStop = nullptr);
public:
	/** 사망 시 이동·캡슐 OFF. NotifyDeath에서 호출. */
	void HandleDeathPrepare();
	/** ABP Death 시퀀스 끝 Notify: 포즈 고정. */
	void OnDeathAnimSequenceEnded();

	/** 레거시/폴백: 포즈 고정 + 연출 시작. */
	void OnDeathAnimationFinished();
	/** 알파/디졸브 연출 시작. AnimNotify에서도 호출 가능(1회 가드). */
	UFUNCTION(BlueprintCallable, Category = "Death")
	void BeginDeathPresentation();
	/** 같은 액터 리스폰(보스). Destroy 없음. */
	void ReviveForRespawn();
	bool IsDead() const { return bIsDead; }
	virtual void OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation) override;
	virtual void OnReturnToPool() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void InitSystem() override;
protected:
	/** 애니 고정(일어남 방지). */
	void FinalizeDeath();
	/** 연출 끝: 풀 반환 등. */
	void OnDeathPresentationComplete();
	/** 풀 Spawn 시 살아 있는 상태 + AI 복구. */
	void ResetForPoolSpawn();
	/** 체력·이동·충돌·애니 복원. */
	void RestoreAliveState();
	void UnbindDeathPresentation();
	UFUNCTION()
	void HandleDeathPresentationFinished();
	US1DeathPresentationComponent* GetDeathPresentationComponent() const { return DeathPresentationComponent; }
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	TObjectPtr<US1DeathPresentationComponent> DeathPresentationComponent;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	bool bUsePooling = false;
private:
	bool bIsDead = false;
	bool bDeathPoseFrozen = false;
	bool bDeathPresentationStarted = false;
};