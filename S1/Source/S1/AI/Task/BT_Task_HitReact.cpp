// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Task/BT_Task_HitReact.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AI/S1AIController.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/S1Monster.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/S1GameplayTags.h"
#include "S1Define.h"

UBT_Task_HitReact::UBT_Task_HitReact()
{
	NodeName = TEXT("Hit React");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBT_Task_HitReact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bTaskFinished = false;
	bGetUpTransitionStarted = false;
	LaunchLoopElapsedTime = 0.f;
	CurrentPhase = EHitReactPhase::None;
	ActiveMontage = nullptr;
	CachedOwnerComp = &OwnerComp;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (nullptr == AIController)
	{
		return EBTNodeResult::Failed;
	}

	AS1Monster* Monster = Cast<AS1Monster>(AIController->GetPawn());
	if (nullptr == Monster || Monster->IsDead())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	const uint8 HitTypeValue = BlackboardComponent->GetValueAsEnum(HitTypeKey.SelectedKeyName);
	const uint8 HitDirectionValue = BlackboardComponent->GetValueAsEnum(HitDirectionKey.SelectedKeyName);
	const ES1HitReactType HitType = static_cast<ES1HitReactType>(HitTypeValue);
	const ES1Direction HitDirection = static_cast<ES1Direction>(HitDirectionValue);

	US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(Monster->GetAbilitySystemComponent());
	if (nullptr == ASC)
	{
		return EBTNodeResult::Failed;
	}
	CachedASC = ASC;

	if (false == PrepareForHitReact(Monster, ASC))
	{
		return EBTNodeResult::Failed;
	}

	// Debug
	D(FString::Printf(TEXT("[HitReactTask] HitTypeValue: %d"), HitTypeValue));

	if (HitType == ES1HitReactType::Launch)
	{
		if (false == StartLaunchPhase(Monster, EHitReactPhase::LaunchStart))
		{
			FinishHitReact(OwnerComp, EBTNodeResult::Failed);
			return EBTNodeResult::Failed;
		}
		return EBTNodeResult::InProgress;
	}

	if (HitType != ES1HitReactType::Weak && HitType != ES1HitReactType::Strong)
	{
		FinishHitReact(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	if (false == StartGroundHit(Monster, HitType, HitDirection))
	{
		FinishHitReact(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBT_Task_HitReact::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bTaskFinished)
	{
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	AS1Monster* Monster = AIController ? Cast<AS1Monster>(AIController->GetPawn()) : nullptr;
	if (nullptr == Monster || Monster->IsDead())
	{
		FinishHitReact(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (ActiveMontage == HitMontages.LaunchMontage.Get())
	{
		SyncLaunchPhaseFromSection(Monster);
	}

	if (CurrentPhase == EHitReactPhase::Ground)
	{
		if (false == IsMontagePlaying(Monster))
		{
			FinishHitReact(OwnerComp, EBTNodeResult::Succeeded);
		}
		return;
	}

	if (CurrentPhase == EHitReactPhase::LaunchLoop)
	{
		LaunchLoopElapsedTime += DeltaSeconds;
		const bool bLaunchLoopTimedOut = LaunchLoopElapsedTime >= MaxLaunchLoopDuration;
		if (UCharacterMovementComponent* Movement = Monster->GetCharacterMovement())
		{
			if (Movement->IsMovingOnGround() || bLaunchLoopTimedOut)
			{
				AdvanceLaunchPhase(Monster);
			}
		}
		else if (bLaunchLoopTimedOut)
		{
			AdvanceLaunchPhase(Monster);
		}
		return;
	}

	if (CurrentPhase == EHitReactPhase::LaunchEnd)
	{
		UpdateLaunchEndPhase(Monster);
		return;
	}

	if (CurrentPhase == EHitReactPhase::GetUp)
	{
		if (false == IsMontagePlaying(Monster))
		{
			FinishHitReact(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

EBTNodeResult::Type UBT_Task_HitReact::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AS1Monster* Monster = AIController ? Cast<AS1Monster>(AIController->GetPawn()) : nullptr)
	{
		if (UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr)
		{
			if (ActiveMontage)
			{
				AnimInstance->Montage_Stop(0.2f, ActiveMontage);
			}
		}
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ClearHitStateTags(ASC);
	}

	if (AS1AIController* S1AIController = Cast<AS1AIController>(OwnerComp.GetAIOwner()))
	{
		S1AIController->ClearHitReactRequest();
	}

	bTaskFinished = true;
	bGetUpTransitionStarted = false;
	LaunchLoopElapsedTime = 0.f;
	CurrentPhase = EHitReactPhase::None;
	ActiveMontage = nullptr;
	CachedOwnerComp = nullptr;
	CachedASC = nullptr;
	return EBTNodeResult::Aborted;
}

void UBT_Task_HitReact::ClearHitStateTags(UAbilitySystemComponent* ASC) const
{
	if (nullptr == ASC)
	{
		return;
	}

	ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_React);
	ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_Launch);
	ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_Knockdown);
	ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_GetUp);
}

void UBT_Task_HitReact::AddPhaseStateTag(UAbilitySystemComponent* ASC, EHitReactPhase Phase) const
{
	if (nullptr == ASC)
	{
		return;
	}

	switch (Phase)
	{
	case EHitReactPhase::Ground:
		ASC->AddLooseGameplayTag(S1StateTags::State_Hit_React);
		break;
	case EHitReactPhase::LaunchStart:
	case EHitReactPhase::LaunchTop:
	case EHitReactPhase::LaunchLoop:
		ASC->AddLooseGameplayTag(S1StateTags::State_Hit_Launch);
		break;
	case EHitReactPhase::LaunchEnd:
		ASC->AddLooseGameplayTag(S1StateTags::State_Hit_Knockdown);
		break;
	case EHitReactPhase::GetUp:
		ASC->AddLooseGameplayTag(S1StateTags::State_Hit_GetUp);
		break;
	default:
		break;
	}
}

void UBT_Task_HitReact::RemovePhaseStateTag(UAbilitySystemComponent* ASC, EHitReactPhase Phase) const
{
	if (nullptr == ASC)
	{
		return;
	}

	switch (Phase)
	{
	case EHitReactPhase::Ground:
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_React);
		break;
	case EHitReactPhase::LaunchStart:
	case EHitReactPhase::LaunchTop:
	case EHitReactPhase::LaunchLoop:
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_Launch);
		break;
	case EHitReactPhase::LaunchEnd:
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_Knockdown);
		break;
	case EHitReactPhase::GetUp:
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_GetUp);
		break;
	default:
		break;
	}
}

bool UBT_Task_HitReact::PrepareForHitReact(AS1Monster* Monster, UAbilitySystemComponent* ASC) const
{
	if (nullptr == Monster || nullptr == ASC)
	{
		return false;
	}

	if (US1EnemyLocomotionComponent* LocomotionComponent = Monster->GetLocomotionComponent())
	{
		if (LocomotionComponent->GetApproachState() == EEnemyApproachState::Approaching
			|| LocomotionComponent->GetApproachState() == EEnemyApproachState::Stopping
			|| LocomotionComponent->GetApproachState() == EEnemyApproachState::StopComplete)
		{
			LocomotionComponent->AbortApproach(true);
		}
	}

	if (USkeletalMeshComponent* Mesh = Monster->GetMesh())
	{
		if (US1AnimInstance_EnemyLocomotion* AnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(Mesh->GetAnimInstance()))
		{
			if (AnimInstance->IsPlayingTurnMontage())
			{
				AnimInstance->StopTurnMontage();
			}
		}
	}

	if (AAIController* AIController = Cast<AAIController>(Monster->GetController()))
	{
		AIController->StopMovement();
	}

	if (US1AbilitySystemComponent* S1ASC = Cast<US1AbilitySystemComponent>(ASC))
	{
		S1ASC->CancelAllAbilities();
	}

	if (UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->StopAllMontages(0.f);
	}

	return true;
}

bool UBT_Task_HitReact::StartGroundHit(AS1Monster* Monster, ES1HitReactType HitType, ES1Direction HitDirection)
{
	UAnimMontage* Montage = ResolveGroundMontage(HitType, HitDirection);
	if (nullptr == Montage)
	{
		return false;
	}

	CurrentPhase = EHitReactPhase::Ground;
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ClearHitStateTags(ASC);
		AddPhaseStateTag(ASC, CurrentPhase);
	}

	ActiveMontage = Montage;
	const float Duration = Monster->PlayAnimMontage(Montage);
	return Duration > 0.f;
}

bool UBT_Task_HitReact::StartLaunchPhase(AS1Monster* Monster, EHitReactPhase Phase)
{
	const bool bGetUpPhase = Phase == EHitReactPhase::GetUp;
	UAnimMontage* Montage = bGetUpPhase ? HitMontages.GetUp.Get() : HitMontages.LaunchMontage.Get();
	if (nullptr == Montage)
	{
		return false;
	}

	const FName StartSectionName = bGetUpPhase ? NAME_None : ResolveLaunchSection(Phase);
	if (false == bGetUpPhase && StartSectionName.IsNone())
	{
		return false;
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		if (Phase == EHitReactPhase::LaunchStart)
		{
			ClearHitStateTags(ASC);
		}
		else
		{
			RemovePhaseStateTag(ASC, CurrentPhase);
		}
		AddPhaseStateTag(ASC, Phase);
	}

	CurrentPhase = Phase;
	if (CurrentPhase == EHitReactPhase::LaunchLoop)
	{
		LaunchLoopElapsedTime = 0.f;
	}
	ActiveMontage = Montage;

	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	const bool bJumpWithinLaunchMontage = false == bGetUpPhase
		&& AnimInstance != nullptr
		&& AnimInstance->Montage_IsPlaying(Montage);

	float Duration = 0.f;
	if (bJumpWithinLaunchMontage)
	{
		AnimInstance->Montage_JumpToSection(StartSectionName, Montage);
		Duration = 1.f;
	}
	else
	{
		Duration = Monster->PlayAnimMontage(Montage, 1.f, StartSectionName);
	}

	if (Duration <= 0.f)
	{
		return false;
	}

	if (AnimInstance != nullptr)
	{
		if (false == bGetUpPhase)
		{
			ConfigureLaunchSections(AnimInstance);
		}

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);
	}

	return true;
}

bool UBT_Task_HitReact::StartGetUpPhase(AS1Monster* Monster)
{
	if (bGetUpTransitionStarted)
	{
		return true;
	}

	UAnimMontage* GetUpMontage = HitMontages.GetUp.Get();
	if (nullptr == Monster || nullptr == GetUpMontage)
	{
		return false;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == AnimInstance)
	{
		return false;
	}

	bGetUpTransitionStarted = true;

	if (UAnimMontage* LaunchMontage = HitMontages.LaunchMontage.Get())
	{
		if (AnimInstance->Montage_IsPlaying(LaunchMontage))
		{
			AnimInstance->Montage_Stop(0.f, LaunchMontage);
		}
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		RemovePhaseStateTag(ASC, CurrentPhase);
		AddPhaseStateTag(ASC, EHitReactPhase::GetUp);
	}

	CurrentPhase = EHitReactPhase::GetUp;
	ActiveMontage = GetUpMontage;

	const float Duration = AnimInstance->Montage_Play(
		GetUpMontage,
		1.f,
		EMontagePlayReturnType::MontageLength,
		0.f);

	return Duration > 0.f;
}

void UBT_Task_HitReact::UpdateLaunchEndPhase(AS1Monster* Monster)
{
	if (bGetUpTransitionStarted || nullptr == Monster)
	{
		return;
	}

	UAnimMontage* LaunchMontage = HitMontages.LaunchMontage.Get();
	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == LaunchMontage || nullptr == AnimInstance)
	{
		StartGetUpPhase(Monster);
		return;
	}

	if (false == AnimInstance->Montage_IsPlaying(LaunchMontage))
	{
		StartGetUpPhase(Monster);
		return;
	}

	const FName EndSection = HitMontages.LaunchEndSection;
	if (EndSection.IsNone())
	{
		StartGetUpPhase(Monster);
		return;
	}

	const int32 EndSectionIndex = LaunchMontage->GetSectionIndex(EndSection);
	if (EndSectionIndex == INDEX_NONE)
	{
		StartGetUpPhase(Monster);
		return;
	}

	const float SectionStartTime = LaunchMontage->GetAnimCompositeSection(EndSectionIndex).GetTime();
	const float SectionLength = LaunchMontage->GetSectionLength(EndSectionIndex);
	const float SectionEndTime = SectionStartTime + SectionLength;
	const float CurrentPosition = AnimInstance->Montage_GetPosition(LaunchMontage);

	if (CurrentPosition >= SectionEndTime - 0.05f)
	{
		StartGetUpPhase(Monster);
	}
}

void UBT_Task_HitReact::AdvanceLaunchPhase(AS1Monster* Monster)
{
	switch (CurrentPhase)
	{
	case EHitReactPhase::LaunchStart:
		StartLaunchPhase(Monster, EHitReactPhase::LaunchTop);
		break;
	case EHitReactPhase::LaunchTop:
		StartLaunchPhase(Monster, EHitReactPhase::LaunchLoop);
		break;
	case EHitReactPhase::LaunchLoop:
		StartLaunchPhase(Monster, EHitReactPhase::LaunchEnd);
		break;
	default:
		break;
	}
}

void UBT_Task_HitReact::FinishHitReact(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type Result)
{
	if (bTaskFinished)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		RemovePhaseStateTag(ASC, CurrentPhase);
		ClearHitStateTags(ASC);
	}

	if (AS1AIController* S1AIController = Cast<AS1AIController>(OwnerComp.GetAIOwner()))
	{
		S1AIController->ClearHitReactRequest();
	}

	bTaskFinished = true;
	bGetUpTransitionStarted = false;
	LaunchLoopElapsedTime = 0.f;
	CurrentPhase = EHitReactPhase::None;
	ActiveMontage = nullptr;
	CachedOwnerComp = nullptr;
	CachedASC = nullptr;
	FinishLatentTask(OwnerComp, Result);
}

UAnimMontage* UBT_Task_HitReact::ResolveGroundMontage(ES1HitReactType HitType, ES1Direction HitDirection) const
{
	const TMap<ES1Direction, TObjectPtr<UAnimMontage>>* MontageMap = nullptr;
	if (HitType == ES1HitReactType::Weak)
	{
		MontageMap = &HitMontages.WeakHits;
	}
	else if (HitType == ES1HitReactType::Strong)
	{
		MontageMap = &HitMontages.StrongHits;
	}

	if (nullptr == MontageMap)
	{
		return nullptr;
	}

	if (const TObjectPtr<UAnimMontage>* FoundMontage = MontageMap->Find(HitDirection))
	{
		return FoundMontage->Get();
	}

	return nullptr;
}

FName UBT_Task_HitReact::ResolveLaunchSection(EHitReactPhase Phase) const
{
	switch (Phase)
	{
	case EHitReactPhase::LaunchStart:
		return HitMontages.LaunchStartSection;
	case EHitReactPhase::LaunchTop:
		return HitMontages.LaunchTopSection;
	case EHitReactPhase::LaunchLoop:
		return HitMontages.LaunchLoopSection;
	case EHitReactPhase::LaunchEnd:
		return HitMontages.LaunchEndSection;
	default:
		return NAME_None;
	}
}

void UBT_Task_HitReact::ConfigureLaunchSections(UAnimInstance* AnimInstance) const
{
	UAnimMontage* LaunchMontage = HitMontages.LaunchMontage.Get();
	if (nullptr == AnimInstance || nullptr == LaunchMontage)
	{
		return;
	}

	AnimInstance->Montage_SetNextSection(
		HitMontages.LaunchStartSection,
		HitMontages.LaunchTopSection,
		LaunchMontage);
	AnimInstance->Montage_SetNextSection(
		HitMontages.LaunchTopSection,
		HitMontages.LaunchLoopSection,
		LaunchMontage);
	AnimInstance->Montage_SetNextSection(
		HitMontages.LaunchLoopSection,
		HitMontages.LaunchLoopSection,
		LaunchMontage);
	AnimInstance->Montage_SetNextSection(
		HitMontages.LaunchEndSection,
		HitMontages.LaunchEndSection,
		LaunchMontage);
}

void UBT_Task_HitReact::SyncLaunchPhaseFromSection(AS1Monster* Monster)
{
	UAnimMontage* LaunchMontage = HitMontages.LaunchMontage.Get();
	if (nullptr == Monster || nullptr == LaunchMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == AnimInstance)
	{
		return;
	}

	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(LaunchMontage);
	EHitReactPhase SyncedPhase = CurrentPhase;
	if (CurrentSection == HitMontages.LaunchStartSection)
	{
		SyncedPhase = EHitReactPhase::LaunchStart;
	}
	else if (CurrentSection == HitMontages.LaunchTopSection)
	{
		SyncedPhase = EHitReactPhase::LaunchTop;
	}
	else if (CurrentSection == HitMontages.LaunchLoopSection)
	{
		SyncedPhase = EHitReactPhase::LaunchLoop;
	}
	else if (CurrentSection == HitMontages.LaunchEndSection)
	{
		SyncedPhase = EHitReactPhase::LaunchEnd;
	}

	if (SyncedPhase == CurrentPhase)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		RemovePhaseStateTag(ASC, CurrentPhase);
		AddPhaseStateTag(ASC, SyncedPhase);
	}
	CurrentPhase = SyncedPhase;
}

bool UBT_Task_HitReact::IsMontagePlaying(AS1Monster* Monster) const
{
	if (nullptr == Monster || nullptr == ActiveMontage)
	{
		return false;
	}

	if (UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr)
	{
		return AnimInstance->Montage_IsPlaying(ActiveMontage);
	}

	return false;
}

void UBT_Task_HitReact::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bTaskFinished || Montage != ActiveMontage)
	{
		return;
	}

	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp.Get();
	if (nullptr == OwnerComp)
	{
		return;
	}

	AAIController* AIController = OwnerComp->GetAIOwner();
	AS1Monster* Monster = AIController ? Cast<AS1Monster>(AIController->GetPawn()) : nullptr;
	if (nullptr == Monster)
	{
		FinishHitReact(*OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (bInterrupted)
	{
		FinishHitReact(*OwnerComp, EBTNodeResult::Failed);
		return;
	}

	switch (CurrentPhase)
	{
	case EHitReactPhase::LaunchStart:
	case EHitReactPhase::LaunchTop:
		AdvanceLaunchPhase(Monster);
		break;
	case EHitReactPhase::LaunchLoop:
		break;
	case EHitReactPhase::LaunchEnd:
		if (false == bGetUpTransitionStarted)
		{
			StartGetUpPhase(Monster);
		}
		break;
	default:
		break;
	}
}
