// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "GameplayTagContainer.h"
#include "Interface/S1PoolingInterface.h"
#include "Interface/S1LockOnInterface.h"
#include "S1Monster.generated.h"

class FLifetimeProperty;

class UBehaviorTree;
class UAnimMontage;
class US1DeathPresentationComponent;
class US1EnemyLocomotionComponent;
class US1MonsterReactBridgeComponent;
class UGameplayEffect;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMonsterHasTargetChangedDelegate, AS1Monster*, Monster, bool, bHasTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMonsterReturnedToPoolDelegate, AS1Monster*, Monster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMonsterNameChangedDelegate, AS1Monster*, Monster);

UCLASS()
class S1_API AS1Monster : public AS1Character, public IS1PoolingInterface, public IS1LockOnInterface
{
	GENERATED_BODY()

public:
	AS1Monster();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Landed(const FHitResult& Hit) override;

public:
	UBehaviorTree*		GetBehaviorTree()	const { return BehaviorTree; }
	bool				IsDead()				const { return bIsDead; }
	bool				HasTarget()				const { return bHasTarget; }
	FGameplayTag		GetDropTableTag()	const { return DropTableTag; }
	const FText&		GetMonsterName()	const { return MonsterName; }
	void				SetMonsterName(const FText& InMonsterName);

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

	/** 서버 권위 몬스터 몽타주 재생. 루트모션은 RepRootMotion(엔진)으로, 제자리 몽타주는 Multicast로 데디 클라에 복제.
	 *  반환: 서버 기준 재생 길이(초). */
	float PlayMonsterMontage(UAnimMontage* Montage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void EnableAttackCollision(const FGameplayTag& CollisionTag, TSubclassOf<UGameplayEffect> DamageEffect, float DamageRatio);
	void DisableAttackCollision(const FGameplayTag& CollisionTag);
	bool ApplyAttackDamage(AActor* TargetActor, TSubclassOf<UGameplayEffect> DamageEffect, float DamageRatio, const FHitResult& HitResult);

	void NotifyHasTargetChanged(bool bInHasTarget);
	US1EnemyLocomotionComponent* GetLocomotionComponent() const { return EnemyLocomotionComponent; }

	/** 서버 전용: Locomotion ABP 상태를 클라이언트에 복제한다. */
	void SetReplicatedLocomotionState(EEnemyLocomotionMode Mode, EEnemyLocomotionPhase Phase, bool bInLocomotionLoop);

	/** 공용 공중 판별: 현재 낙하(공중) 중인지. 다른 패턴/BT에서도 사용. */
	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsAirborne() const;

	/** 점프 발사 직후 호출(서버). 시작 높이 기록 + 정점 감지 무장 + State.Enemy.Air 부여.
	 *  ApexHeight: 시작점 대비 목표 정점 높이. Tick에서 이 높이 도달 시 Event.Enemy.Apex 전송. */
	void EnterAirborneState(float ApexHeight);

	/** 시간 정지(궁극기 등) — 서버에서 호출, 전체 클라에 CustomTimeDilation 적용해 애니메이션까지 정지 */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTimeFrozen(bool bFrozen);

public:
	UPROPERTY(BlueprintAssignable)
	FMonsterHasTargetChangedDelegate OnHasTargetChanged;

	UPROPERTY(BlueprintAssignable)
	FMonsterReturnedToPoolDelegate OnReturnedToPool;

	UPROPERTY(BlueprintAssignable)
	FMonsterNameChangedDelegate OnMonsterNameChanged;

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

	UFUNCTION()
	void OnRep_IsDead();

	UFUNCTION()
	void OnRep_HasTarget();

	UFUNCTION()
	void OnRep_MonsterName();

	UFUNCTION()
	void OnRep_ReplicatedLocomotionState();

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

	UPROPERTY(ReplicatedUsing = OnRep_MonsterName, EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	FText MonsterName;

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

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = { false };

	UPROPERTY(ReplicatedUsing = OnRep_HasTarget)
	bool bHasTarget = { false };

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
	EEnemyLocomotionMode ReplicatedLocomotionMode = EEnemyLocomotionMode::None;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
	EEnemyLocomotionPhase ReplicatedLocomotionPhase = EEnemyLocomotionPhase::None;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
	bool bReplicatedLocomotionLoop = false;

	bool bDeathPoseFrozen = { false };
	bool bDeathPresentationStarted = { false };

	/** 공중 점프 상태(서버 Tick에서 정점 감지용). */
	bool bApexPending = { false };
	float AirborneStartZ = { 0.f };
	float TargetApexZ = { 0.f };

	TMap<FGameplayTag, FS1ActiveAttackCollision> ActiveAttackCollisions;
	TMap<FGameplayTag, TArray<TWeakObjectPtr<AActor>>> AttackCollisionHitActors;
};
