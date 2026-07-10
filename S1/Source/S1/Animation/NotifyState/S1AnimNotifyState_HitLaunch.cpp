// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_HitLaunch.h"

#include "Character/S1Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Engine/World.h"
#include "S1LogChannels.h"

namespace
{
	static const FName HitLaunchRootMotionName(TEXT("S1HitLaunch"));
}

void US1AnimNotifyState_HitLaunch::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AS1Character* Character = Cast<AS1Character>(MeshComp ? MeshComp->GetOwner() : nullptr);
	if (false == IsValid(Character))
	{
		LOG(TEXT("[HitLaunch] NotifyBegin — Character invalid (MeshComp owner가 AS1Character가 아님)"));
		return;
	}

	LOG(TEXT("[HitLaunch] NotifyBegin fired — %s, TotalDuration: %f, PendingVelocity: %s"),
		*Character->GetName(), TotalDuration, *Character->GetPendingHitLaunchVelocity().ToString());

	// 데디 서버 NotifyState thrash 방어(§AtkCollision과 동일 원인) — 직전 적용 후 짧은 시간 내 재발화면 중복으로 보고 스킵
	// (여기서 걸러야 함 — 안 걸면 아래 회전 캐시가 이미 꺼진 값을 "원래값"으로 잘못 저장해 원복이 깨짐)
	const float CurrentTime = Character->GetWorld() ? Character->GetWorld()->GetTimeSeconds() : 0.f;
	const float LastAppliedTime = Character->GetLastHitLaunchAppliedTime();
	if (LastAppliedTime >= 0.f && (CurrentTime - LastAppliedTime) < 0.05f)
	{
		LOG(TEXT("[HitLaunch] 디바운스로 스킵 — %f초 전에 이미 적용됨 (thrash 의심)"), CurrentTime - LastAppliedTime);
		return;
	}
	Character->SetLastHitLaunchAppliedTime(CurrentTime);

	// ⚠️ MoveEvent와 달리 시뮬 프록시도 배제하지 않음 — 시뮬 프록시가 위치 복제 보간만으로 따라가면
	// 넉백처럼 짧고 빠른 이동은 스냅샷 사이가 벌어져 순간이동처럼 보임. 몽타주 자체가 이미 전체 복제되므로
	// 이 노티파이도 전체 머신에서 로컬로 동일하게 발화 → 다 같이 결정론적으로 같은 힘을 적용
	UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
	if (nullptr == CMC)
	{
		return;
	}

	// 회전 스냅이 넉백 이동 중 유지되도록 이동 중 재조준 잠시 해제 (§Evasion과 동일 이유 — 없으면 CMC가 매 틱 이동 방향으로 되돌림)
	Character->SetCachedOrientRotationToMovement(CMC->bOrientRotationToMovement);
	CMC->bOrientRotationToMovement = false;
	Character->SetActorRotation(Character->GetPendingHitFacingDirection().Rotation());

	const FVector LaunchVelocity = Character->GetPendingHitLaunchVelocity();
	if (LaunchVelocity.IsNearlyZero() || TotalDuration <= KINDA_SMALL_NUMBER)
	{
		LOG(TEXT("[HitLaunch] 힘 적용 안 함 — LaunchVelocity nearly zero: %d, TotalDuration: %f"),
			LaunchVelocity.IsNearlyZero(), TotalDuration);
		return;
	}

	LOG(TEXT("[HitLaunch] ApplyRootMotionSource 호출 — Force: %s, Duration: %f, MaintainOnFinish: %d"),
		*LaunchVelocity.ToString(), TotalDuration, bMaintainVelocityOnFinish);

	// Walking 모드는 캐릭터를 바닥에 붙어있는 것으로 보고 매 틱 Z를 바닥으로 되돌림(§LaunchCharacter와 동일 함정) →
	// 포물선(위/옆으로 뜨는) 넉백이려면 먼저 Falling으로 전환해야 루트모션이 실제로 반영됨
	if (CMC->MovementMode != MOVE_Falling)
	{
		CMC->SetMovementMode(MOVE_Falling);
	}
	LOG(TEXT("[HitLaunch] SetMovementMode 후 실제 MovementMode: %d (3=Falling이어야 정상)"), (int32)CMC->MovementMode.GetValue());

	TSharedPtr<FRootMotionSource_ConstantForce> Force = MakeShared<FRootMotionSource_ConstantForce>();
	Force->InstanceName        = HitLaunchRootMotionName;
	Force->AccumulateMode      = ERootMotionAccumulateMode::Override;
	Force->Priority            = 5;
	Force->Force               = LaunchVelocity;
	Force->Duration            = TotalDuration;

	// ToAir Start 구간: 구간이 끝나는 순간 속도를 유지 → 이후 CMC Falling 중력이 자연스럽게 포물선으로 낙하시킴
	// Strong 등: 구간 끝나면 정지
	if (bMaintainVelocityOnFinish)
	{
		Force->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
	}
	else
	{
		Force->FinishVelocityParams.Mode        = ERootMotionFinishVelocityMode::SetVelocity;
		Force->FinishVelocityParams.SetVelocity = FVector::ZeroVector;
	}

	CMC->ApplyRootMotionSource(Force);
}

void US1AnimNotifyState_HitLaunch::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AS1Character* Character = Cast<AS1Character>(MeshComp ? MeshComp->GetOwner() : nullptr);
	if (false == IsValid(Character))
	{
		return;
	}

	// 조기 중단(몽타주 인터럽트) 대비 — 정상 종료 시엔 이미 만료. NotifyBegin과 동일하게 전체 머신에서 정리
	if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
	{
		CMC->RemoveRootMotionSource(HitLaunchRootMotionName);
		CMC->bOrientRotationToMovement = Character->GetCachedOrientRotationToMovement();
	}
	Character->ClearPendingHitLaunchVelocity();
}
