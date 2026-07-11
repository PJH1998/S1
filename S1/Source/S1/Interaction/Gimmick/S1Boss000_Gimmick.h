// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Gimmick/S1GimmickObject.h"
#include "GameplayTagContainer.h"
#include "Engine/TimerHandle.h"
#include "S1Boss000_Gimmick.generated.h"

class UStaticMeshComponent;

/**
 * Boss000 전용 기믹 기둥.
 *  - 보스의 특정 공격 콜리전(CC_Weapon)과 직접 충돌하면 부서진 메쉬 2개로 교체.
 *  - 보스가 올라타 발돋움하는 발판으로 사용(GetJumpTopLocation).
 */
UCLASS()
class S1_API AS1Boss000_Gimmick : public AS1GimmickObject
{
	GENERATED_BODY()

public:
	AS1Boss000_Gimmick();

	// 보스가 올라설 기둥 상단 위치(소켓 우선, 없으면 바운즈 상단).
	UFUNCTION(BlueprintPure, Category = "Gimmick")
	FVector GetJumpTopLocation() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_IsActivated() override;

	UFUNCTION()
	void OnIntactMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// 파괴/복구 상태에 맞춰 메쉬 가시성·발판 콜리전 동기화(서버·클라 공용).
	void ApplyBrokenVisual(bool bBroken);

	// 파괴 연출: 파편을 분리·물리 시뮬레이션·임펄스 후 FragmentLifetime 뒤 소멸(코스메틱, 각 머신 로컬).
	void PlayBreakFx();
	// FragmentLifetime 만료: 파편 숨김 + 물리 정지.
	void OnFragmentLifetimeExpired();
	// 리셋: 파편 물리 정지·재부착·원위치·숨김.
	void RestoreFragments();

private:
	// 온전한 상태 메쉬(보스 발판 콜리전 겸용).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> IntactMesh;

	// 부서진 조각 메쉬 2종.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BreakMesh01;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BreakMesh02;

	// 비어 있지 않으면, 이 태그를 컴포넌트 태그로 가진 보스 공격 콜리전만 파괴를 유발한다(특정 공격 한정).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AcceptedBreakTags;

	// 지정 시 상단 발판 위치로 사용할 소켓.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick", meta = (AllowPrivateAccess = "true"))
	FName JumpTopSocketName = NAME_None;

	// 파편 임펄스 세기(velocity change, cm/s), 상향 가중치, 소멸 시간(0이면 리셋까지 유지).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Break", meta = (AllowPrivateAccess = "true"))
	float FragmentImpulseStrength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Break", meta = (AllowPrivateAccess = "true"))
	float FragmentUpBias = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick|Break", meta = (AllowPrivateAccess = "true"))
	float FragmentLifetime = 3.f;

	// 파편 초기 상대 트랜스폼(리셋 복원용) — BeginPlay에서 캡처.
	FTransform BreakMesh01InitialRelative;
	FTransform BreakMesh02InitialRelative;

	FTimerHandle FragmentTimerHandle;
};
