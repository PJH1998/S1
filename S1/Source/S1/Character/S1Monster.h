// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "GameplayTagContainer.h"
#include "Interface/S1PoolingInterface.h"
#include "Interface/S1LockOnInterface.h"
#include "S1Monster.generated.h"

class UBehaviorTree;
class UAnimMontage;
class US1DeathPresentationComponent;
class US1EnemyLocomotionComponent;
class US1MonsterReactBridgeComponent;
class UGameplayEffect;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMonsterHasTargetChangedDelegate, AS1Monster*, Monster, bool, bHasTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMonsterReturnedToPoolDelegate, AS1Monster*, Monster);

UCLASS()
class S1_API AS1Monster : public AS1Character, public IS1PoolingInterface, public IS1LockOnInterface
{
	GENERATED_BODY()

public:
	AS1Monster();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UBehaviorTree*		GetBehaviorTree()	const { return BehaviorTree; }
	bool				IsDead()				const { return bIsDead; }
	FGameplayTag		GetDropTableTag()	const { return DropTableTag; }

public:
	virtual void Tick(float DeltaTime) override;
	virtual void InitSystem() override;

	virtual void OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation) override;
	virtual void OnReturnToPool() override;

	virtual ES1EnemyTier	GetLockOnTier_Implementation()      override;
	virtual FVector			GetLockOnFocusLocation_Implementation() override;

public:
	/** HP 0 시 1회: bIsDead, 어빌리티 취소, BB bIsDead (BT는 유지). */
	virtual void NotifyDeath();

	void PlayAnimation(UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	void PlaySpawnAnimation();

	void EnableAttackCollision(const FGameplayTag& CollisionTag, TSubclassOf<UGameplayEffect> DamageEffect, float DamageRatio);
	void DisableAttackCollision(const FGameplayTag& CollisionTag);
	bool ApplyAttackDamage(AActor* TargetActor, TSubclassOf<UGameplayEffect> DamageEffect, float DamageRatio);

	void NotifyHasTargetChanged(bool bInHasTarget);
	US1EnemyLocomotionComponent* GetLocomotionComponent() const { return EnemyLocomotionComponent; }

public:
	UPROPERTY(BlueprintAssignable)
	FMonsterHasTargetChangedDelegate OnHasTargetChanged;

	UPROPERTY(BlueprintAssignable)
	FMonsterReturnedToPoolDelegate OnReturnedToPool;

public:
	/** 사망 시 이동·캡슐 OFF. NotifyDeath에서 호출. */
	void HandleDeathPrepare();
	/** ABP Death 시퀀스 끝 Notify: 포즈 고정. */
	void OnDeathAnimSequenceEnded();
	/** 데스 포즈 고정. MontageToStop 지정 시 해당 AM만 정지(중복 안전). */
	void FreezeDeathPose(UAnimMontage* MontageToStop = nullptr);
	/** 레거시/폴백: 포즈 고정 + 연출 시작. */
	void OnDeathAnimationFinished();
	/** 알파/디졸브 연출 시작. AnimNotify에서도 호출 가능(1회 가드). */
	UFUNCTION(BlueprintCallable, Category = "Death")
	void BeginDeathPresentation();
	/** 같은 액터 리스폰(보스). Destroy 없음. */
	void ReviveForRespawn();

protected:
	US1DeathPresentationComponent* GetDeathPresentationComponent() const { return DeathPresentationComponent; }

	/** 애니 고정(일어남 방지). */
	void FinalizeDeath();
	/** 연출 끝: 풀 반환 등. */
	void OnDeathPresentationComplete();
	/** 풀 Spawn 시 살아 있는 상태 + AI 복구. */
	void ResetForPoolSpawn();
	/** 체력·이동·충돌·애니 복원. */
	void RestoreAliveState();

	void UnbindDeathPresentation();
	void ResumeAIAfterSpawnAnimation();

protected:
	void InitializeAttackCollisions();
	void UninitializeAttackCollisions();
	UPrimitiveComponent* FindAttackCollisionComponent(const FGameplayTag& CollisionTag) const;

	UFUNCTION()
	void OnAttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleDeathPresentationFinished();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	TObjectPtr<US1DeathPresentationComponent> DeathPresentationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
	TObjectPtr<US1EnemyLocomotionComponent> EnemyLocomotionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitReact")
	TObjectPtr<US1MonsterReactBridgeComponent> ReactBridgeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AttackCollision")
	FGameplayTagContainer AttackCollisionTags;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	FName LockOnFocusBone = FName("VFX_Center");

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	bool bUsePooling = { false };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	FGameplayTag DropTableTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TObjectPtr<UAnimMontage> SpawnMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
	float SpawnAnimationPlayRate = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	bool bBlockAIWhileSpawnAnimation = true;

private:
	struct FS1ActiveAttackCollision
	{
		TSubclassOf<UGameplayEffect> DamageEffect;
		float DamageRatio = 1.f;
	};

	bool bIsDead = { false };
	bool bHasTarget = { false };
	bool bDeathPoseFrozen = { false };
	bool bDeathPresentationStarted = { false };

	TMap<FGameplayTag, FS1ActiveAttackCollision> ActiveAttackCollisions;
	TMap<FGameplayTag, TArray<TWeakObjectPtr<AActor>>> AttackCollisionHitActors;
};
