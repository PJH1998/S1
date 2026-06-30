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
};
