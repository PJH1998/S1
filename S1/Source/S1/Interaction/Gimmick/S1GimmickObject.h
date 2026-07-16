// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "S1GimmickObject.generated.h"

class FLifetimeProperty;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS1GimmickActivatedCosmeticSignature);

/**
 * 모든 보스/월드 기믹 오브젝트의 공통 베이스.
 * US1GimmickManager에 GimmickTag로 self-register 하며, 활성화/복구 상태(bIsActivated)를 복제한다.
 * 메쉬 스왑·물리 등 연출은 자식이 OnRep_IsActivated를 override 하여 채운다.
 */
UCLASS(Abstract)
class S1_API AS1GimmickObject : public AActor
{
	GENERATED_BODY()

public:
	AS1GimmickObject();

	FGameplayTag GetGimmickTag() const { return GimmickTag; }
	bool IsUsable() const { return false == bIsActivated; }

	// 클라 전용 연출(VFX/사운드/카메라쉐이크/어나운스 등) BP 바인딩 전용. 활성화 전환 시(리셋 전환 제외)
	// 로컬 플레이어가 있는 머신(스탠드얼론/리슨서버 호스트/각 클라)에서 정확히 1회씩 발화하며,
	// 데디케이티드 서버에서는 발화하지 않는다(서버가 원격 PC의 카메라/입력을 건드리는 사고 방지).
	// 드랍/퀘스트 같은 서버 권위 로직은 여기 바인딩하지 말 것 — 기존 OnPuzzleCompleted류(서버 전용) 델리게이트를 그대로 쓴다.
	UPROPERTY(BlueprintAssignable, Category = "Gimmick")
	FS1GimmickActivatedCosmeticSignature OnActivatedCosmetic;

	// 서버 전용: 활성화 상태로 전환(멱등). 상태/복제는 베이스가 처리, 연출은 자식 OnRep에서.
	virtual void ActivateGimmick();
	// 서버 전용: 원상 복구(멱등).
	virtual void ResetGimmick();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 클라 시각화 훅(자식 override). 베이스는 상태만 보유.
	UFUNCTION()
	virtual void OnRep_IsActivated();

protected:
	// 매니저 등록/질의 식별 태그.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gimmick")
	FGameplayTag GimmickTag;

	UPROPERTY(ReplicatedUsing = OnRep_IsActivated)
	bool bIsActivated = false;
};
