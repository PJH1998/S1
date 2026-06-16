// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "S1HitReactTypes.h"
#include "BT_Task_HitReact.generated.h"

class UAnimMontage;
class UAnimInstance;
class UAbilitySystemComponent;
class AS1Monster;

UENUM()
enum class EHitReactPhase : uint8
{
	None,
	Ground,
	LaunchStart,
	LaunchTop,
	LaunchLoop,
	LaunchEnd,
	GetUp,
};

UCLASS()
class S1_API UBT_Task_HitReact : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBT_Task_HitReact();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void ClearHitStateTags(UAbilitySystemComponent* ASC) const;
	void AddPhaseStateTag(UAbilitySystemComponent* ASC, EHitReactPhase Phase) const;
	void RemovePhaseStateTag(UAbilitySystemComponent* ASC, EHitReactPhase Phase) const;
	bool PrepareForHitReact(AS1Monster* Monster, UAbilitySystemComponent* ASC) const;
	bool StartGroundHit(AS1Monster* Monster, ES1HitReactType HitType, ES1Direction HitDirection);
	bool StartLaunchPhase(AS1Monster* Monster, EHitReactPhase Phase);
	void AdvanceLaunchPhase(AS1Monster* Monster);
	void FinishHitReact(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result);
	UAnimMontage* ResolveGroundMontage(ES1HitReactType HitType, ES1Direction HitDirection) const;
	FName ResolveLaunchSection(EHitReactPhase Phase) const;
	void ConfigureLaunchSections(UAnimInstance* AnimInstance) const;
	void SyncLaunchPhaseFromSection(AS1Monster* Monster);
	bool IsMontagePlaying(AS1Monster* Monster) const;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PendingHitReactKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HitTypeKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HitDirectionKey;

	UPROPERTY(EditAnywhere, Category = "Hit|Animation")
	FS1HitReactMontages HitMontages;

	bool bTaskFinished = false;
	EHitReactPhase CurrentPhase = EHitReactPhase::None;
	TObjectPtr<UAnimMontage> ActiveMontage;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
};
