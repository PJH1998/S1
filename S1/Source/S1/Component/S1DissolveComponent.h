// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "S1DissolveComponent.generated.h"

class UMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_MULTICAST_DELEGATE(FOnS1DissolveComplete)

// 무기 등 임의 메시의 Dissolve 머티리얼 파라미터를 틱으로 애니메이션 — MF_ApperanceDissolve 계열 머티리얼 전제
// (파라미터 "Dissolve" 1=완전히 보임 / 0=완전히 사라짐)
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1DissolveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1DissolveComponent();

	// TargetMesh의 머티리얼에서 DissolveParameterName MID를 캐시하고 bAppear 방향으로 Duration에 걸쳐 애니메이션
	// bAppear=true: 0(사라짐)→1(보임), false: 1(보임)→0(사라짐)
	void PlayDissolve(UMeshComponent* TargetMesh, bool bAppear, float Duration);

	FOnS1DissolveComplete OnDissolveComplete;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void InitializeMIDs(UMeshComponent* TargetMesh);
	void ApplyDissolveAmount(float Amount);

	UPROPERTY(EditDefaultsOnly, Category = "Dissolve")
	FName DissolveParameterName = TEXT("Dissolve");

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DissolveMIDs;

	float ElapsedTime = 0.f;
	float PlayDuration = 1.f;
	float StartValue = 1.f;
	float EndValue = 1.f;
	bool bPlaying = false;
};
