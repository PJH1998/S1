// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S1InteractComponent.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS1NearestInteractableChangedSignature, AActor*, NewNearest);

/**
 * 플레이어 주변의 IS1InteractableInterface 구현 액터를 오버랩으로 탐지한다.
 * US1LockOnComponent와 동일한 스피어+후보집합 패턴 — 오버랩 자체는 각 머신에서 로컬로
 * 굴러가는 순수 탐지용이라 서버 인증이 필요 없다(실제 상태 변경은 AS1Player::ServerInteract에서 처리).
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class S1_API US1InteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1InteractComponent();

	// 탐지된 후보 중 가장 가까운 상호작용 대상(없으면 nullptr). CanInteract()가 false인 후보는 제외한다.
	AActor* GetNearestInteractable() const;

	// 가장 가까운 상호작용 대상이 바뀔 때만 브로드캐스트(HUD 프롬프트 표시용).
	UPROPERTY(BlueprintAssignable, Category = "Interact")
	FS1NearestInteractableChangedSignature OnNearestInteractableChanged;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 가장 가까운 대상을 다시 계산해 이전과 다르면 OnNearestInteractableChanged를 브로드캐스트한다.
	void UpdateNearestInteractable();

private:
	// 뷰포트에서 보이고 에디터에서 채널 설정 가능
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	TObjectPtr<USphereComponent> InteractSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractRadius = 200.f;

	TSet<TWeakObjectPtr<AActor>> CandidateSet;
	TWeakObjectPtr<AActor> CachedNearest;
};
