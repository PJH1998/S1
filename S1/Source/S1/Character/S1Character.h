// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "S1Character.generated.h"

class US1AbilitySystemComponent;
class US1AttributeSet;
class UAnimMontage;
class FLifetimeProperty;
class UNiagaraComponent;

UCLASS()
class S1_API AS1Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AS1Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void InitSystem();

	// 서버 권위 GA의 몽타주를 전체 클라(소유자 포함)에서 재생 — ASC 복제는 소유 클라를 스킵하므로 Multicast 사용
	// 서버에서도 동기 실행되어 Progression의 종료/섹션 델리게이트가 서버에서 정상 동작
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMontage(UAnimMontage* Montage, float Rate, FName StartSection);

	// 재생 중인 몽타주의 섹션 점프를 전체 클라(+서버)에 복제 — Montage_JumpToSection은 서버 AnimInstance에만 적용되어 데디 클라 미반영
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpToSection(UAnimMontage* Montage, FName SectionName);

	// GravityScale을 복제해서 설정 — 서버 권위 GA가 중력을 바꿔도 소유 클라 CMC 예측이 일치(호버 덜덜 방지)
	// 서버에서 호출. 전체 클라(소유자 포함)에 복제되어 OnRep에서 CMC에 적용
	void SetReplicatedGravityScale(float Scale);

	// 피격 넉백 속도/회전 방향을 몽타주 재생 트리거 "직전"에 명시적으로 멀티캐스트 — US1AnimNotifyState_HitLaunch가
	// 전체 머신에서 읽어 루트모션/회전에 사용. ⚠️ 일반 Replicated 프로퍼티였다가 실측에서 레이스 확인됨(§ 아래) →
	// 같은 액터의 Reliable Multicast끼리는 도착 순서가 보장되므로, 이 호출을 몽타주 재생(MulticastPlayMontage 등) 직전에
	// 명시적으로 호출하면 노티파이가 발화하는 시점엔 항상 값이 이미 세팅되어 있음이 보장됨
	// (예전엔 별도 Replicated 프로퍼티로 전달 — 몽타주 Multicast와 다른 복제 채널이라 도착 순서 보장이 안 돼서
	//  일부 머신에서 노티파이가 먼저 발화해 값이 0으로 읽히는 레이스가 실측으로 확인됨)
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetHitLaunchData(FVector Velocity, FVector Facing);

	FVector GetPendingHitLaunchVelocity() const { return PendingHitLaunchVelocity; }
	FVector GetPendingHitFacingDirection() const { return PendingHitFacingDirection; }

	// 넉백 종료 후 로컬 정리(Notify NotifyEnd에서 각 머신이 자기 값만 리셋) — 순서 보장 불필요라 멀티캐스트 아님
	void ClearPendingHitLaunchVelocity() { PendingHitLaunchVelocity = FVector::ZeroVector; }

	// CMC->bOrientRotationToMovement 캐시(넉백 이동 중 회전 스냅 유지용) — Notify는 몽타주에 박힌 공유 UObject라
	// 재생 주체별 상태를 못 들고 있어서(Effects_Niagara.md §29 경고) Character에 대신 보관
	void SetCachedOrientRotationToMovement(bool bValue) { bCachedOrientRotationToMovement = bValue; }
	bool GetCachedOrientRotationToMovement() const { return bCachedOrientRotationToMovement; }

	// 소켓에 붙여 재생 중인 이펙트를 EffectKey로 등록 — 같은 Key로 이전 이펙트가 끝나기 전에 또 스폰될 수 있어 Key당 배열(큐)로 보관
	// Replicated 아님 — 각 머신이 자기 로컬 Niagara 인스턴스만 추적(몽타주 자체가 이미 서버/클라 각각 재생되므로 이 함수도 각 머신에서 로컬로 호출됨)
	void RegisterAttachedEffect(FGameplayTag EffectKey, UNiagaraComponent* Component);

	// EffectKey로 등록된 이펙트 중 가장 먼저 등록된(=먼저 시작된) 것부터 종료 — Begin/End가 순서대로 쌍을 이룬다는 전제(FIFO)
	// 현재 소켓에 부착돼 있으면(GetAttachParent() 확인) Detach 후 정리, 부착된 적 없으면 그냥 Map에서만 제거 — 호출부가 Attach 여부를 몰라도 됨
	// LifetimeAfterEnd가 0보다 크면 그 시간(초) 뒤에 강제로 Deactivate — NS 자체 Duration/Loop 설정이 잘못돼도 확실히 정리되는 안전장치
	void EndAttachedEffect(FGameplayTag EffectKey, float LifetimeAfterEnd = 0.f);

	// EffectKey로 등록된 이펙트 목록 조회(제거 없이 Peek) — NotifyTick 등에서 활성 인스턴스에 값을 계속 흘려보낼 때 사용
	const TArray<TWeakObjectPtr<UNiagaraComponent>>* FindAttachedEffects(FGameplayTag EffectKey) const;

	// bAutoRelease=false로 스폰돼 스스로 안 사라지는 컴포넌트를, 더 이상 재사용 안 할 시점(예: Charge 마지막 사이클)에 호출
	// 스폰 시점이 아니라 "진짜 끝났다"고 판단되는 시점에 직접 걸어야 함 — 스폰 시 걸면 Once+Burst NS는 매 재트리거(Activate)마다
	// OnSystemFinished가 발화해서 첫 사이클만에 파괴돼버림. Infinite NS는 이 이벤트가 런타임에 발화 안 하므로 걸어도 무해
	void ArmEffectAutoDestroy(UNiagaraComponent* Component);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_GravityScale();

	UFUNCTION()
	void OnManagedEffectFinished(UNiagaraComponent* FinishedComponent);

	UPROPERTY(ReplicatedUsing = OnRep_GravityScale)
	float RepGravityScale = 1.f;

	// MulticastSetHitLaunchData로 전체 머신에 동일하게 세팅 — 일반 Replicated 프로퍼티 아님(레이스 방지, 위 설명 참조)
	FVector PendingHitLaunchVelocity = FVector::ZeroVector;
	FVector PendingHitFacingDirection = FVector::ForwardVector;

	// 각 머신 로컬 스크래치 — 복제 불필요(Notify가 그 머신에서 껐다 그 머신에서 복원)
	bool bCachedOrientRotationToMovement = true;

public:
	// 데디 서버 NotifyState thrash 방어(§AtkCollision과 동일 원인 — AlwaysTickPoseAndRefreshBones 비렌더 서버에서
	// NotifyBegin/End가 매 프레임 재발화) — 직전 적용 시각과 너무 가까우면 중복 적용으로 보고 스킵
	float GetLastHitLaunchAppliedTime() const { return LastHitLaunchAppliedTime; }
	void SetLastHitLaunchAppliedTime(float Time) { LastHitLaunchAppliedTime = Time; }

private:
	float LastHitLaunchAppliedTime = -1.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1AttributeSet> AttributeSet;

	// TWeakObjectPtr는 GC에 안전해 UPROPERTY 불필요 — UHT가 TMap 값으로 TArray를 직접 반영하지 못하는 제약도 우회됨
	TMap<FGameplayTag, TArray<TWeakObjectPtr<UNiagaraComponent>>> ActiveAttachedEffects;
};
