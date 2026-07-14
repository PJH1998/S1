// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1GameplayAbility_Attack.generated.h"

class US1AbilityTask_RotateToCamera;

UCLASS()
class S1_API US1GameplayAbility_Attack : public US1GameplayAbility_Action
{
	GENERATED_BODY()

public:
	US1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	// Progression 콤보 어드밴스 시 카메라 회전 갱신
	virtual void OnProgressionMontageStarted() override;

	// 몽타주 재생 시 — AtkCollision 노티파이 스캔 → 서버 히트 윈도우 타이머 스케줄
	virtual void OnAbilityMontagePlayed(UAnimMontage* Montage, float Rate) override;

protected:
	void StartRotateToCamera();

protected:
	// 특정 상황에서 재발동 방지를 위한 Tag
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag UsedTag;

	// 기본 데미지 GE
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// 추가 디버프 GE — 설정 시 DamageEffect와 함께 적용
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DebuffEffect;

	// US1AbilityTask_RotateToCamera 실행 여부
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	bool bRotateToCamera = false;

	// US1AbilityTask_RotateToCamera 회전 속도
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (EditCondition = "bRotateToCamera"))
	float RotationSpeed = 720.f;


private:
	void BindAttackBox();
	void UnbindAttackBox();

	void ResetMainHitTargets();
	void ResetOffhandHitTargets();

	UFUNCTION()
	void OnAttackBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 서버 권위에서만 — 몽타주의 AtkCollision 윈도우(begin/end)를 real-time 타이머로 스케줄
	void ScheduleHitWindows(UAnimMontage* Montage, float Rate);
	void ClearHitWindowTimers();
	void EnableWeaponHitCollision(float AtkScale, FGameplayTag HitStrengthTag, FGameplayTag SoundBaseTag, ES1AttackHand Hand);
	void DisableWeaponHitCollision(ES1AttackHand Hand);

	// AtkCollision 노티파이의 VirtualShape — 무기와 같은 AtkScale/HitStrengthTag를 공유하는 추가 판정 도형(Player 공용 Sphere/Box 1개씩)
	void EnableVirtualHitCollision(ES1AtkCollisionShape Shape, ES1EffectAttachTarget AttachTarget, FName SocketName, const FTransform& SpawnOffset, float SphereRadius, const FVector& BoxExtent, float AtkScale, FGameplayTag HitStrengthTag, FGameplayTag SoundBaseTag, ES1AttackHand Hand);
	void DisableVirtualHitCollision();

	UPROPERTY()
	TObjectPtr<US1AbilityTask_RotateToCamera> RotateTask;

	// Main/Offhand 독립 히트 타겟 — 같은 타겟을 양손으로 개별 타격 가능 (가상 충돌체 히트도 여기 합류해 무기와 합쳐서 대상당 1회 보장)
	TArray<TWeakObjectPtr<AActor>> MainHitTargets;
	TArray<TWeakObjectPtr<AActor>> OffhandHitTargets;

	// 활성 히트 윈도우 타이머 (몽타주당 begin/end 쌍) — 몽타주 교체/종료 시 정리
	TArray<FTimerHandle> HitWindowTimers;

	// 가상 충돌체가 활성 중일 때 OnAttackBoxOverlap이 참조할 값 — EnableVirtualHitCollision에서 세팅, Disable에서 리셋
	float         PendingVirtualAtkScale = 1.0f;
	FGameplayTag  PendingVirtualHitStrengthTag;
	FGameplayTag  PendingVirtualSoundBaseTag;
	ES1AttackHand ActiveVirtualHand = ES1AttackHand::Main;

	// 반구 판정용 — Sphere 콜리전 + 전방 180도 DotProduct 필터로 구현(엔진에 반구 프리미티브 없음)
	bool    bVirtualHemisphereActive = false;
	FVector VirtualHemisphereOrigin = FVector::ZeroVector;
	FVector VirtualHemisphereForward = FVector::ForwardVector;
};
