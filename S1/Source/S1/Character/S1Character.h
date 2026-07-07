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

	// 소켓에 붙여 재생 중인 이펙트를 EffectKey로 등록 — 같은 Key로 이전 이펙트가 끝나기 전에 또 스폰될 수 있어 Key당 배열(큐)로 보관
	// Replicated 아님 — 각 머신이 자기 로컬 Niagara 인스턴스만 추적(몽타주 자체가 이미 서버/클라 각각 재생되므로 이 함수도 각 머신에서 로컬로 호출됨)
	void RegisterAttachedEffect(FGameplayTag EffectKey, UNiagaraComponent* Component);

	// EffectKey로 등록된 이펙트 중 가장 먼저 등록된(=먼저 시작된) 것부터 소켓에서 분리(현재 월드 트랜스폼 유지) — Begin/End가 순서대로 쌍을 이룬다는 전제(FIFO)
	void DetachAttachedEffect(FGameplayTag EffectKey);

	// 애초에 소켓에 붙은 적 없는 이펙트용 — Detach 없이 가장 먼저 등록된 것부터 Map에서만 제거(FIFO)
	void RemoveAttachedEffect(FGameplayTag EffectKey);

	// EffectKey로 등록된 이펙트 목록 조회(제거 없이 Peek) — NotifyTick 등에서 활성 인스턴스에 값을 계속 흘려보낼 때 사용
	const TArray<TWeakObjectPtr<UNiagaraComponent>>* FindAttachedEffects(FGameplayTag EffectKey) const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_GravityScale();

	UPROPERTY(ReplicatedUsing = OnRep_GravityScale)
	float RepGravityScale = 1.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1AttributeSet> AttributeSet;

	// TWeakObjectPtr는 GC에 안전해 UPROPERTY 불필요 — UHT가 TMap 값으로 TArray를 직접 반영하지 못하는 제약도 우회됨
	TMap<FGameplayTag, TArray<TWeakObjectPtr<UNiagaraComponent>>> ActiveAttachedEffects;
};
