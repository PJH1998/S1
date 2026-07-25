// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Enemy/Boss000/S1GA_Boss000_WallKick.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AI/S1AIController.h"
#include "Animation/AnimInstance.h"
#include "Character/Boss/Boss000/S1Boss_000.h"
#include "Character/Player/S1Player.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/S1AnimData.h"
#include "Engine/World.h"
#include "Interaction/Gimmick/S1Boss000_Gimmick.h"
#include "Kismet/GameplayStatics.h"
#include "System/S1AssetManager.h"
#include "System/S1GimmickManager.h"

US1GA_Boss000_WallKick::US1GA_Boss000_WallKick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1GA_Boss000_WallKick::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AS1Boss_000* Monster = GetMonster();
	US1GimmickManager* GimmickManager = GetGimmickManager();
	if (nullptr == Monster || nullptr == GimmickManager)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	HopCount = 0;
	TargetHops = FMath::RandRange(FMath::Min(MinHops, MaxHops), FMath::Max(MinHops, MaxHops));

	CurrentPillar = Cast<AS1Boss000_Gimmick>(GimmickManager->GetRandomUsableGimmick(GimmickTag));
	if (false == CurrentPillar.IsValid())
	{
		// 사용할 기둥이 없으면 패턴 불가 — 즉시 종료.
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 패턴 동안 중력 off — 홉 사이에 기둥 위에 머물게. EndAbility에서 복원.
	// 첫 홉은 바닥→기둥이라 무시할 "현재 기둥"이 아직 없음 — 다음 Move/Slash 페이즈 시작 시점에 그때의 CurrentPillar(=착지한 기둥)를 무시 처리.
	BeginPatternMovement();

	const FVector FirstTarget = CurrentPillar->GetJumpTopLocation();
	Monster->SetWallKickTarget(FirstTarget);
	FaceTowards(FirstTarget);
	CurrentPhase = EWallKickPhase::Start;
	PlayPhaseMontage(StartMontageTag);
}

void US1GA_Boss000_WallKick::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	EndPatternMovement();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void US1GA_Boss000_WallKick::OnPhaseCompleted()
{
	AS1Boss_000* Monster = GetMonster();
	if (nullptr == Monster)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (CurrentPhase == EWallKickPhase::Slash)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Move 한 번을 끝냈으면 카운트 증가. (Start는 카운트하지 않음)
	if (CurrentPhase == EWallKickPhase::Move)
	{
		++HopCount;
	}

	// 다음 페이즈를 이전 몽타주 블렌드아웃 중에 시작(크로스페이드, Idle 제거).
	// 이때 이전 태스크의 자기-중단 콜백이 어빌리티를 끝내지 않도록 가드.
	bAdvancing = true;
	if (HopCount < TargetHops)
	{
		if (false == StartMovePhase())
		{
			StartSlashPhase();
		}
	}
	else
	{
		StartSlashPhase();
	}
	bAdvancing = false;
}

void US1GA_Boss000_WallKick::OnPhaseInterrupted()
{
	// 페이즈 전환 중 발생한 자기-중단(이전 몽타주를 의도적으로 끊고 다음 것 재생)은 무시.
	if (bAdvancing)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void US1GA_Boss000_WallKick::PlayPhaseMontage(const FGameplayTag& PhaseMontageTag)
{
	UAnimMontage* Montage = FindPhaseMontage(PhaseMontageTag);
	if (nullptr == Montage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ActiveMontage = Montage;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, 1.f, NAME_None);
	if (nullptr == MontageTask)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 완료 시 다음 페이즈로 체이닝, 중단/취소 시 안전 종료. (base PlayAttackMontage는 완료 시 무조건 EndAbility라 미사용)
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnPhaseCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnPhaseInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnPhaseInterrupted);
	MontageTask->ReadyForActivation();
}

bool US1GA_Boss000_WallKick::StartMovePhase()
{
	AS1Boss_000* Monster = GetMonster();
	US1GimmickManager* GimmickManager = GetGimmickManager();
	if (nullptr == Monster || nullptr == GimmickManager)
	{
		return false;
	}

	AS1Boss000_Gimmick* NextPillar = Cast<AS1Boss000_Gimmick>(GimmickManager->GetRandomUsableGimmick(GimmickTag, CurrentPillar.Get()));
	if (nullptr == NextPillar)
	{
		return false;
	}

	// 지금 밟고 서 있는(출발할) 기둥만 무시 대상으로 — 다음 값으로 덮어쓰기 전에 캡처.
	SetIgnoredPillar(CurrentPillar.Get());
	CurrentPillar = NextPillar;
	const FVector NextTarget = NextPillar->GetJumpTopLocation();
	Monster->SetWallKickTarget(NextTarget);
	FaceTowards(NextTarget);   // 이동 즉시 다음 기둥을 바라보게
	CurrentPhase = EWallKickPhase::Move;
	PlayPhaseMontage(MoveMontageTag);
	return true;
}

void US1GA_Boss000_WallKick::StartSlashPhase()
{
	AS1Boss_000* Monster = GetMonster();
	if (nullptr == Monster)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UAnimMontage* Montage = FindPhaseMontage(SlashMontageTag);
	UAnimInstance* AnimInstance = Monster->GetMesh() ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == Montage || nullptr == AnimInstance)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 마지막으로 밟고 있던 기둥에서 플레이어 쪽으로 이탈 — 그 기둥만 무시 대상으로.
	SetIgnoredPillar(CurrentPillar.Get());

	CurrentPhase = EWallKickPhase::Slash;
	ActiveMontage = Montage;
	const FVector SlashTarget = ResolveTargetLocation();
	Monster->SetWallKickTarget(SlashTarget);
	FaceTowards(SlashTarget);

	// Start 섹션부터 재생 + 자체 완료/중단 바인딩(체이닝).
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, 1.f, SlashStartSection);
	if (nullptr == MontageTask)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnPhaseCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnPhaseInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnPhaseInterrupted);
	MontageTask->ReadyForActivation();

	// 가까워지기 전까지 Loop 반복. CheckDistance 이벤트에서 Loop→End로 바꾼다.
	AnimInstance->Montage_SetNextSection(SlashStartSection, SlashLoopSection, Montage);
	AnimInstance->Montage_SetNextSection(SlashLoopSection, SlashLoopSection, Montage);

	if (CheckDistanceEventTag.IsValid())
	{
		UAbilityTask_WaitGameplayEvent* DistanceTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CheckDistanceEventTag);
		DistanceTask->EventReceived.AddDynamic(this, &ThisClass::OnSlashCheckDistance);
		DistanceTask->ReadyForActivation();
	}
}

void US1GA_Boss000_WallKick::OnSlashCheckDistance(FGameplayEventData Payload)
{
	AS1Boss_000* Monster = GetMonster();
	UAnimInstance* AnimInstance = (Monster && Monster->GetMesh()) ? Monster->GetMesh()->GetAnimInstance() : nullptr;
	if (nullptr == AnimInstance || nullptr == ActiveMontage)
	{
		return;
	}

	const FVector TargetLocation = ResolveTargetLocation();
	Monster->SetWallKickTarget(TargetLocation);   // 다음 루프 dash가 추적

	if (FVector::Dist2D(Monster->GetActorLocation(), TargetLocation) <= SlashApproachDistance)
	{
		AnimInstance->Montage_SetNextSection(SlashLoopSection, SlashEndSection, ActiveMontage);
	}
}

UAnimMontage* US1GA_Boss000_WallKick::FindPhaseMontage(const FGameplayTag& PhaseMontageTag) const
{
	US1AnimData* AnimData = US1AssetManager::GetAssetByTag<US1AnimData>(AnimDataTag);
	if (nullptr == AnimData)
	{
		return nullptr;
	}

	const FS1MontageData* MontageData = AnimData->FindMontageByTag(PhaseMontageTag);
	return MontageData ? MontageData->Montage : nullptr;
}

AS1Boss_000* US1GA_Boss000_WallKick::GetMonster() const
{
	return Cast<AS1Boss_000>(GetAvatarActorFromActorInfo());
}

US1GimmickManager* US1GA_Boss000_WallKick::GetGimmickManager() const
{
	if (const AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (UWorld* World = Avatar->GetWorld())
		{
			return World->GetSubsystem<US1GimmickManager>();
		}
	}

	return nullptr;
}

FVector US1GA_Boss000_WallKick::ResolveTargetLocation() const
{
	const AS1Boss_000* Monster = GetMonster();
	if (nullptr == Monster)
	{
		return FVector::ZeroVector;
	}

	if (const AS1AIController* AIController = Cast<AS1AIController>(Monster->GetController()))
	{
		if (const AActor* TargetActor = AIController->GetTargetActor())
		{
			return TargetActor->GetActorLocation();
		}
	}

	return Monster->GetActorLocation() + Monster->GetActorForwardVector() * 500.f;
}

void US1GA_Boss000_WallKick::BeginPatternMovement()
{
	if (bPatternMovementActive)
	{
		return;
	}

	AS1Boss_000* Monster = GetMonster();
	if (nullptr == Monster)
	{
		return;
	}

	SetGravityScale(0.f);
	IgnoreAllPlayers(true);

	bPatternMovementActive = true;
}

void US1GA_Boss000_WallKick::EndPatternMovement()
{
	if (false == bPatternMovementActive)
	{
		return;
	}

	ResetGravityScale();
	SetIgnoredPillar(nullptr);
	IgnoreAllPlayers(false);

	bPatternMovementActive = false;
}

void US1GA_Boss000_WallKick::IgnoreAllPlayers(bool bShouldIgnore)
{
	AS1Boss_000* Monster = GetMonster();
	UCapsuleComponent* Capsule = Monster ? Monster->GetCapsuleComponent() : nullptr;
	if (nullptr == Capsule)
	{
		return;
	}

	if (bShouldIgnore)
	{
		TArray<AActor*> Players;
		UGameplayStatics::GetAllActorsOfClass(Monster->GetWorld(), AS1Player::StaticClass(), Players);
		for (AActor* Player : Players)
		{
			Capsule->IgnoreActorWhenMoving(Player, true);
			IgnoredPlayers.Add(Player);
		}
	}
	else
	{
		for (const TWeakObjectPtr<AActor>& Player : IgnoredPlayers)
		{
			if (Player.IsValid())
			{
				Capsule->IgnoreActorWhenMoving(Player.Get(), false);
			}
		}
		IgnoredPlayers.Empty();
	}
}

void US1GA_Boss000_WallKick::SetIgnoredPillar(AS1Boss000_Gimmick* NewPillar)
{
	if (IgnoredPillar.Get() == NewPillar)
	{
		return;
	}

	AS1Boss_000* Monster = GetMonster();
	UCapsuleComponent* Capsule = Monster ? Monster->GetCapsuleComponent() : nullptr;

	if (Capsule && IgnoredPillar.IsValid())
	{
		Capsule->IgnoreActorWhenMoving(IgnoredPillar.Get(), false);
	}

	IgnoredPillar = NewPillar;

	if (Capsule && NewPillar)
	{
		Capsule->IgnoreActorWhenMoving(NewPillar, true);
	}
}

void US1GA_Boss000_WallKick::FaceTowards(const FVector& TargetLocation) const
{
	AS1Boss_000* Monster = GetMonster();
	if (nullptr == Monster)
	{
		return;
	}

	FVector Direction = TargetLocation - Monster->GetActorLocation();
	Direction.Z = 0.f;
	if (false == Direction.Normalize())
	{
		return;
	}

	FRotator NewRotation = Monster->GetActorRotation();
	NewRotation.Yaw = Direction.Rotation().Yaw;
	Monster->SetActorRotation(NewRotation);
}
