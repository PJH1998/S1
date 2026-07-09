// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_MoveEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"

#include "S1LogChannels.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"

namespace
{
	// 노티파이 루트모션 소스 식별자 (조기 중단 시 이름으로 제거)
	static const FName MoveEventRootMotionName(TEXT("S1MoveEvent"));
}

void US1AnimNotifyState_MoveEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ACharacter* Character = Cast<ACharacter>(MeshComp ? MeshComp->GetOwner() : nullptr);
	if (false == IsValid(Character))
	{
		return;
	}

	// 1) 서버 GA 로직용 이벤트 (히트 콜리전 타이밍 등) — 이동은 더 이상 GA가 처리 안 함
	if (MoveBeginEventTag.IsValid())
	{
		FGameplayEventData Payload;
		if (TotalDuration > KINDA_SMALL_NUMBER)
		{
			Payload.EventMagnitude = MoveDistance / TotalDuration;
		}
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, MoveBeginEventTag, Payload);
	}

	// AbilityTask가 이동을 처리하는 경우(LocalPredicted) 노티파이는 이벤트 전달만 — 이중 이동 방지
	if (bUseAbilityTaskMovement)
	{
		return;
	}

	// 2) 이동 — 소유 클라(예측) + 서버(검증) + 시뮬 프록시 전부 로컬로 동일 루트모션 추가
	//    소유 클라만 추가하면 서버가 같은 소스를 몰라 클라 이동을 "비정상"으로 보고 되돌림(보정)
	//    → 양쪽이 동일 소스를 가져야 서버가 클라 예측을 인정.
	//    ⚠️ 시뮬 프록시도 배제하지 않음 — 위치 복제 보간만으로는 짧고 빠른 이동이 스냅샷 사이가 벌어져
	//    순간이동처럼 보임(§HitLaunch에서 실측). 몽타주 자체가 이미 전체 복제되므로 이 노티파이도
	//    전체 머신에서 로컬로 동일하게 발화해 결정론적으로 같은 힘을 적용
	if (FMath::IsNearlyZero(MoveDistance) || TotalDuration <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
	if (nullptr == CMC)
	{
		return;
	}

	const float Speed = MoveDistance / TotalDuration;	// 음수 = 후방 이동

	TSharedPtr<FRootMotionSource_ConstantForce> Force = MakeShared<FRootMotionSource_ConstantForce>();
	Force->InstanceName        = MoveEventRootMotionName;
	Force->AccumulateMode      = ERootMotionAccumulateMode::Override;
	Force->Priority            = 5;
	Force->Force               = Character->GetActorForwardVector() * Speed;
	Force->Duration            = TotalDuration;
	Force->FinishVelocityParams.Mode        = ERootMotionFinishVelocityMode::SetVelocity;
	Force->FinishVelocityParams.SetVelocity = FVector::ZeroVector;

	CMC->ApplyRootMotionSource(Force);
}

void US1AnimNotifyState_MoveEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ACharacter* Character = Cast<ACharacter>(MeshComp ? MeshComp->GetOwner() : nullptr);
	if (false == IsValid(Character))
	{
		return;
	}

	if (MoveEndEventTag.IsValid())
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, MoveEndEventTag, Payload);
	}

	// AbilityTask 처리 어빌리티는 GA가 MoveEnd 이벤트 수신 시 태스크를 종료 — 노티파이는 소스 제거 불필요
	if (bUseAbilityTaskMovement)
	{
		return;
	}

	// 조기 중단(몽타주 인터럽트) 대비 — 전체 머신에서 루트모션 소스 제거 (정상 종료 시엔 이미 만료)
	if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
	{
		CMC->RemoveRootMotionSource(MoveEventRootMotionName);
	}
}
