// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/S1Monster.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/S1AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/S1AttributeSet.h"
#include "AI/S1AIController.h"
#include "Animation/S1AnimInstance_EnemyLocomotion.h"
#include "AIController.h"
#include "Component/S1DeathPresentationComponent.h"
#include "Component/S1EnemyLocomotionComponent.h"
#include "Component/S1MonsterHPBarComponent.h"
#include "Component/S1MonsterReactBridgeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "S1Define.h"
#include "Tags/S1GameplayTags.h"
#include "System/S1DropManager.h"
#include "System/S1MonsterManager.h"

AS1Monster::AS1Monster()
	: Super()
{
	bReplicates = true;
	SetReplicateMovement(true);

	AbilitySystemComponent = CreateDefaultSubobject<US1AbilitySystemComponent>("AbilitySystemComponent");
	DeathPresentationComponent = CreateDefaultSubobject<US1DeathPresentationComponent>(TEXT("DeathPresentationComponent"));
	EnemyLocomotionComponent = CreateDefaultSubobject<US1EnemyLocomotionComponent>(TEXT("EnemyLocomotionComponent"));
	ReactBridgeComponent = CreateDefaultSubobject<US1MonsterReactBridgeComponent>(TEXT("ReactBridgeComponent"));
}

void AS1Monster::BeginPlay()
{
	Super::BeginPlay();
	InitSystem();
	InitializeAttackCollisions();

	if (US1MonsterManager* MonsterManager = GetWorld()->GetSubsystem<US1MonsterManager>())
	{
		MonsterManager->RegisterMonster(this);
	}
}

void AS1Monster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (US1MonsterManager* MonsterManager = GetWorld()->GetSubsystem<US1MonsterManager>())
	{
		MonsterManager->UnregisterMonster(this);
	}

	UninitializeAttackCollisions();
	ActiveAttackCollisions.Empty();
	AttackCollisionHitActors.Empty();

	if (US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->CancelAllAbilities();
	}

	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->StopPresentation();
	}

	OnHasTargetChanged.Clear();
	bHasTarget = false;

	Super::EndPlay(EndPlayReason);
}

void AS1Monster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS1Monster, bIsDead);
	DOREPLIFETIME(AS1Monster, bHasTarget);
	DOREPLIFETIME(AS1Monster, ReplicatedLocomotionMode);
	DOREPLIFETIME(AS1Monster, ReplicatedLocomotionPhase);
	DOREPLIFETIME(AS1Monster, bReplicatedLocomotionLoop);
}

void AS1Monster::SetReplicatedLocomotionState(EEnemyLocomotionMode Mode, EEnemyLocomotionPhase Phase, bool bInLocomotionLoop)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (ReplicatedLocomotionMode == Mode
		&& ReplicatedLocomotionPhase == Phase
		&& bReplicatedLocomotionLoop == bInLocomotionLoop)
	{
		return;
	}

	ReplicatedLocomotionMode = Mode;
	ReplicatedLocomotionPhase = Phase;
	bReplicatedLocomotionLoop = bInLocomotionLoop;
	ForceNetUpdate();
}

void AS1Monster::OnRep_ReplicatedLocomotionState()
{
	if (HasAuthority())
	{
		return;
	}

	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (US1AnimInstance_EnemyLocomotion* EnemyAnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(SkeletalMeshComp->GetAnimInstance()))
		{
			EnemyAnimInstance->ApplyReplicatedLocomotionState(ReplicatedLocomotionMode, ReplicatedLocomotionPhase, bReplicatedLocomotionLoop);
		}
	}
}

void AS1Monster::PlayAnimation(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}

void AS1Monster::PlaySpawnAnimation()
{
	UAnimMontage* MontageToPlay = SpawnMontage.Get();
	if (MontageToPlay == nullptr)
	{
		return;
	}

	if (bBlockAIWhileSpawnAnimation)
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
			if (AS1AIController* S1AIController = Cast<AS1AIController>(AIController))
			{
				S1AIController->StopAIForDeath();
			}
		}
	}

	const float PlayRate = FMath::Max(SpawnAnimationPlayRate, KINDA_SMALL_NUMBER);
	const float Duration = PlayAnimMontage(MontageToPlay, PlayRate);
	if (bBlockAIWhileSpawnAnimation == false || Duration <= 0.f)
	{
		ResumeAIAfterSpawnAnimation();
		return;
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::ResumeAIAfterSpawnAnimation, Duration, false);
}

void AS1Monster::EnableAttackCollision(const FGameplayTag& CollisionTag, TSubclassOf<UGameplayEffect> DamageEffect, float DamageRatio)
{
	if (false == HasAuthority())
	{
		return;
	}

	UPrimitiveComponent* CollisionComponent = FindAttackCollisionComponent(CollisionTag);
	if (CollisionComponent == nullptr)
	{
		return;
	}

	ActiveAttackCollisions.FindOrAdd(CollisionTag) = FS1ActiveAttackCollision{ DamageEffect, DamageRatio };
	AttackCollisionHitActors.FindOrAdd(CollisionTag).Reset();
 	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->UpdateOverlaps();
}

void AS1Monster::DisableAttackCollision(const FGameplayTag& CollisionTag)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (UPrimitiveComponent* CollisionComponent = FindAttackCollisionComponent(CollisionTag))
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	ActiveAttackCollisions.Remove(CollisionTag);
	AttackCollisionHitActors.Remove(CollisionTag);
}

bool AS1Monster::ApplyAttackDamage(AActor* TargetActor, TSubclassOf<UGameplayEffect> DamageEffect, float DamageRatio, const FHitResult& HitResult)
{
	if (false == HasAuthority())
	{
		return false;
	}

	if (false == IsValid(TargetActor) || TargetActor == this || DamageEffect == nullptr)
	{
		return false;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (SourceASC == nullptr || TargetASC == nullptr)
	{
		return false;
	}

	const US1AttributeSet* AttribSet = Cast<US1AttributeSet>(SourceASC->GetAttributeSet(US1AttributeSet::StaticClass()));
	if (AttribSet == nullptr)
	{
		return false;
	}

	const float FinalDamage = AttribSet->GetBaseDamage() * DamageRatio;
	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	EffectContext.AddHitResult(HitResult, true);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, EffectContext);
	if (false == SpecHandle.IsValid())
	{
		return false;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(S1SetByCallerTags::SetByCaller_Damage, -FinalDamage);
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}

void AS1Monster::InitializeAttackCollisions()
{
	for (const FGameplayTag& CollisionTag : AttackCollisionTags)
	{
		UPrimitiveComponent* CollisionComponent = FindAttackCollisionComponent(CollisionTag);
		if (CollisionComponent == nullptr)
		{
			continue;
		}

		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnAttackCollisionBeginOverlap);
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackCollisionBeginOverlap);
	}
}

void AS1Monster::UninitializeAttackCollisions()
{
	for (const FGameplayTag& CollisionTag : AttackCollisionTags)
	{
		UPrimitiveComponent* CollisionComponent = FindAttackCollisionComponent(CollisionTag);
		if (CollisionComponent == nullptr)
		{
			continue;
		}

		CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnAttackCollisionBeginOverlap);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

UPrimitiveComponent* AS1Monster::FindAttackCollisionComponent(const FGameplayTag& CollisionTag) const
{
	if (false == CollisionTag.IsValid())
	{
		return nullptr;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	const FName ComponentTagName = CollisionTag.GetTagName();
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (PrimitiveComponent && PrimitiveComponent->ComponentHasTag(ComponentTagName))
		{
			return PrimitiveComponent;
		}
	}

	return nullptr;
}

void AS1Monster::OnAttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (bIsDead || false == IsValid(OtherActor) || OtherActor == this || OtherComp == nullptr)
	{
		return;
	}

	if (OtherComp->GetCollisionObjectType() != S1CollisionChannel::CC_Player)
	{
		return;
	}

	FGameplayTag CollisionTag;
	for (const FGameplayTag& AttackCollisionTag : AttackCollisionTags)
	{
		if (OverlappedComp->ComponentHasTag(AttackCollisionTag.GetTagName()))
		{
			CollisionTag = AttackCollisionTag;
			break;
		}
	}

	if (false == CollisionTag.IsValid())
	{
		return;
	}

	TArray<TWeakObjectPtr<AActor>>& HitActors = AttackCollisionHitActors.FindOrAdd(CollisionTag);
	for (const TWeakObjectPtr<AActor>& HitActor : HitActors)
	{
		if (HitActor.Get() == OtherActor)
		{
			return;
		}
	}

	const FS1ActiveAttackCollision* ActiveCollision = ActiveAttackCollisions.Find(CollisionTag);
	if (ActiveCollision == nullptr)
	{
		return;
	}

	HitActors.Add(OtherActor);
	ApplyAttackDamage(OtherActor, ActiveCollision->DamageEffect, ActiveCollision->DamageRatio, SweepResult);
}

// HP 0: bIsDeadAnim·물리·BT 정지. 데스 재생·연출 타이밍은 ABP + AnimNotify.
void AS1Monster::NotifyDeath()
{
	if (false == HasAuthority())
	{
		return;
	}

	if (bIsDead)
	{
		return;
	}
	bIsDead = true;
	// AnimState 갱신
	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (US1AnimInstance_EnemyLocomotion* EnemyAnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(SkeletalMeshComp->GetAnimInstance()))
		{
			EnemyAnimInstance->SetDeadAnimState(true);
		}
	}
	if (bHasTarget)
	{
		NotifyHasTargetChanged(false);
	}
	// Ability Cancel
	if (US1AbilitySystemComponent* ASC = Cast<US1AbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->CancelAllAbilities();
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_React);
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_Launch);
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_Knockdown);
		ASC->RemoveLooseGameplayTag(S1StateTags::State_Hit_GetUp);
	}
	// Movement Stop
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		if (AS1AIController* S1AIController = Cast<AS1AIController>(AIController))
		{
			S1AIController->SetBlackboardIsDead(true);
		}
	}
	HandleDeathPrepare();
	// Drop Spawn
	if (US1DropManager* DropManager = GetWorld()->GetSubsystem<US1DropManager>())
	{
		DropManager->HandleMonsterDeath(this);
	}

	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->ClearHitReactRequest();
		S1AIController->StopAIForDeath();
	}

	ForceNetUpdate();
}

void AS1Monster::HandleDeathPrepare()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AS1Monster::OnDeathAnimSequenceEnded()
{
	if (bIsDead == false)
	{
		return;
	}

	FinalizeDeath();
}

void AS1Monster::OnDeathAnimationFinished()
{
	OnDeathAnimSequenceEnded();
	BeginDeathPresentation();
}

void AS1Monster::FreezeDeathPose(UAnimMontage* MontageToStop)
{
	if (bDeathPoseFrozen)
	{
		return;
	}

	bDeathPoseFrozen = true;

	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance())
		{
			if (MontageToStop)
			{
				AnimInstance->Montage_Stop(0.f, MontageToStop);
			}
			else if (UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage())
			{
				AnimInstance->Montage_Stop(0.f, ActiveMontage);
			}
		}

		SkeletalMeshComp->bPauseAnims = true;
	}
}

void AS1Monster::FinalizeDeath()
{
	FreezeDeathPose();
}

void AS1Monster::BeginDeathPresentation()
{
	if (bDeathPresentationStarted || bIsDead == false)
	{
		return;
	}
	bDeathPresentationStarted = true;
	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->OnPresentationComplete.AddDynamic(this, &AS1Monster::HandleDeathPresentationFinished);
		DeathPresentation->StartPresentation();
		return;
	}
	OnDeathPresentationComplete();
}

// 연출 완료 델리게이트 수신용. OnDeathPresentationComplete로 넘긴다.
void AS1Monster::HandleDeathPresentationFinished()
{
	OnDeathPresentationComplete();
}

// 연출 종료: 델리게이트 해제 후 bUsePooling이면 풀 반환(Destroy 없음).
void AS1Monster::OnDeathPresentationComplete()
{
	UnbindDeathPresentation();
	if (bUsePooling && HasAuthority())
	{
		ReturnSelf();
	}
}

// 보스 등 같은 액터 리스폰: 사망 상태·연출·AI·스탯을 살아 있는 상태로 되돌린다.
void AS1Monster::ReviveForRespawn()
{
	if (false == HasAuthority())
	{
		return;
	}

	if (bIsDead == false)
	{
		return;
	}
	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->StopPresentation();
	}
	RestoreAliveState();
	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->ResetBlackboardForSpawn();
		S1AIController->ResumeAIAfterRevive();
	}

	ForceNetUpdate();
}

// 풀에서 꺼낼 때(IS1PoolingInterface). 위치/표시 복구 후 ResetForPoolSpawn.
void AS1Monster::OnSpawnFromPool(FGameplayTag InPoolTag, FVector Location, FRotator Rotation)
{
	IS1PoolingInterface::OnSpawnFromPool(InPoolTag, Location, Rotation);
	ResetForPoolSpawn();

	if (US1MonsterManager* MonsterManager = GetWorld()->GetSubsystem<US1MonsterManager>())
	{
		MonsterManager->RegisterMonster(this);
	}

	ForceNetUpdate();
}

// 풀에 넣을 때(IS1PoolingInterface). 연출 중단 후 Hidden·충돌 OFF.
void AS1Monster::OnReturnToPool()
{
	if (US1MonsterManager* MonsterManager = GetWorld()->GetSubsystem<US1MonsterManager>())
	{
		MonsterManager->UnregisterMonster(this);
	}

	UnbindDeathPresentation();
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->StopPresentation();
	}
	OnReturnedToPool.Broadcast(this);
	IS1PoolingInterface::OnReturnToPool();
	ForceNetUpdate();
}

// 풀 재사용 직전: 살아 있는 상태 복구 + AI 블랙보드/BT 재개.
void AS1Monster::ResetForPoolSpawn()
{
	RestoreAliveState();

	if (US1MonsterHPBarComponent* HPBarComponent = FindComponentByClass<US1MonsterHPBarComponent>())
	{
		HPBarComponent->ResetForPoolSpawn();
	}

	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->ResetBlackboardForSpawn();
		S1AIController->ResumeAIAfterRevive();
	}
}

// bIsDead 해제, 체력·이동·캡슐·스켈레탈 애니를 전투 가능 상태로 복원.
void AS1Monster::RestoreAliveState()
{
	bIsDead = false;
	bDeathPoseFrozen = false;
	bDeathPresentationStarted = false;
	bHasTarget = false;
	SetActorHiddenInGame(false);
	if (US1AttributeSet* MonsterAttributeSet = Cast<US1AttributeSet>(AttributeSet))
	{
		MonsterAttributeSet->SetHealth(MonsterAttributeSet->GetMaxHealth());
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		SkeletalMeshComp->bPauseAnims = false;
		SkeletalMeshComp->InitAnim(true);
		if (UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance())
		{
			if (US1AnimInstance_EnemyLocomotion* EnemyAnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(AnimInstance))
			{
				EnemyAnimInstance->SetDeadAnimState(false);
				EnemyAnimInstance->ResetLocomotion();
			}
		}
	}

	SetReplicatedLocomotionState(EEnemyLocomotionMode::None, EEnemyLocomotionPhase::None, false);
}

// DeathPresentation 완료 델리게이트 바인딩 해제(풀/리스폰/재사망 시 중복 방지).
void AS1Monster::UnbindDeathPresentation()
{
	if (US1DeathPresentationComponent* DeathPresentation = GetDeathPresentationComponent())
	{
		DeathPresentation->OnPresentationComplete.RemoveDynamic(this, &AS1Monster::HandleDeathPresentationFinished);
	}
}

void AS1Monster::ResumeAIAfterSpawnAnimation()
{
	if (bIsDead)
	{
		return;
	}

	if (AS1AIController* S1AIController = Cast<AS1AIController>(GetController()))
	{
		S1AIController->ResetBlackboardForSpawn();
		S1AIController->ResumeAIAfterRevive();
	}
}

void AS1Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 점프 상승 중 정점 도달 감지(서버). 상승 구간은 결정론적이라 높이 임계값으로 판별.
	if (bApexPending && HasAuthority())
	{
		const bool bReachedHeight = (GetActorLocation().Z >= TargetApexZ);
		const bool bDescending = (GetVelocity().Z <= 0.f);
		if (bReachedHeight || bDescending)
		{
			bApexPending = false;

			if (IsValid(AbilitySystemComponent))
			{
				FGameplayEventData EventData;
				AbilitySystemComponent->HandleGameplayEvent(S1EventTags::Event_Enemy_Apex, &EventData);
			}
		}
	}
}

bool AS1Monster::IsAirborne() const
{
	const UCharacterMovementComponent* Movement = GetCharacterMovement();
	return IsValid(Movement) && Movement->IsFalling();
}

void AS1Monster::EnterAirborneState(float ApexHeight)
{
	if (false == HasAuthority() || false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	AirborneStartZ = GetActorLocation().Z;
	TargetApexZ = AirborneStartZ + ApexHeight;
	bApexPending = true;

	AbilitySystemComponent->AddLooseGameplayTag(S1StateTags::State_Enemy_Air);
}

void AS1Monster::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (false == HasAuthority() || false == IsValid(AbilitySystemComponent))
	{
		return;
	}

	bApexPending = false;
	AbilitySystemComponent->RemoveLooseGameplayTag(S1StateTags::State_Enemy_Air);

	FGameplayEventData EventData;
	AbilitySystemComponent->HandleGameplayEvent(S1EventTags::Event_Enemy_Landed, &EventData);
}

void AS1Monster::InitSystem()
{
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AS1Monster::NotifyHasTargetChanged(bool bInHasTarget)
{
	if (false == HasAuthority())
	{
		return;
	}

	if (bHasTarget == bInHasTarget)
	{
		return;
	}

	bHasTarget = bInHasTarget;
	OnHasTargetChanged.Broadcast(this, bHasTarget);
	ForceNetUpdate();
}

void AS1Monster::OnRep_IsDead()
{
	if (bIsDead)
	{
		if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
		{
			if (US1AnimInstance_EnemyLocomotion* EnemyAnimInstance = Cast<US1AnimInstance_EnemyLocomotion>(SkeletalMeshComp->GetAnimInstance()))
			{
				EnemyAnimInstance->SetDeadAnimState(true);
			}
		}

		HandleDeathPrepare();
		BeginDeathPresentation();
		return;
	}

	RestoreAliveState();
}

void AS1Monster::OnRep_HasTarget()
{
	OnHasTargetChanged.Broadcast(this, bHasTarget);
}

ES1EnemyTier AS1Monster::GetLockOnTier_Implementation()
{
	return ES1EnemyTier::Normal;
}

FVector AS1Monster::GetLockOnFocusLocation_Implementation()
{
	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (false == ::IsValid(SkelMesh) || SkelMesh->GetBoneIndex(LockOnFocusBone) == INDEX_NONE)
	{
		return GetActorLocation();
	}
	return SkelMesh->GetBoneLocation(LockOnFocusBone);
}
