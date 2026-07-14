// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_AtkCollision.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#if WITH_EDITOR
#include "PrimitiveDrawingUtils.h"
#endif

// 데이터 마커 — 동작 없음. 값은 GA_Attack이 몽타주 스캔으로 읽어 서버 히트 윈도우를 구동한다.

FS1AtkCollisionData US1AnimNotifyState_AtkCollision::GetCollisionData() const
{
	FS1AtkCollisionData Data;
	Data.AtkScale             = AtkScale;
	Data.HitStrengthTag       = HitStrengthTag;
	Data.SoundBaseTag         = SoundBaseTag;
	Data.AttackHand           = AttackHand;
	Data.bEnableWeaponCollision = bEnableWeaponCollision;
	Data.VirtualShape         = VirtualShape;
	Data.VirtualAttachTarget  = VirtualAttachTarget;
	Data.VirtualSocketName    = VirtualSocketName;
	Data.VirtualSpawnOffset   = VirtualSpawnOffset;
	Data.VirtualSphereRadius  = VirtualSphereRadius;
	Data.VirtualBoxExtent     = VirtualBoxExtent;
	return Data;
}

#if WITH_EDITOR
// 엔진에 반구 프리미티브가 없어 PDI->DrawLine으로 직접 그림 — 적도(잘린 단면) 원 + 전방(Forward=X축) 극까지의 자오선
// Transform의 Forward 방향이 런타임 DotProduct 필터(전방 180도)와 동일한 축이라 시각화와 실제 판정이 정확히 일치함
static void DrawAtkCollisionWireHemisphere(FPrimitiveDrawInterface* PDI, const FTransform& Transform, float Radius, const FLinearColor& Color)
{
	const FVector Origin  = Transform.GetLocation();
	const FVector Forward = Transform.GetRotation().GetForwardVector();
	const FVector Right   = Transform.GetRotation().GetRightVector();
	const FVector Up      = Transform.GetRotation().GetUpVector();

	constexpr int32 NumSides = 16;

	// 적도 원 — 반구가 잘리는 평면(Forward에 수직인 Right-Up 평면)의 경계
	FVector PrevEquator = Origin + Right * Radius;
	for (int32 i = 1; i <= NumSides; ++i)
	{
		const float Theta = 2.f * PI * i / NumSides;
		const FVector NextEquator = Origin + (Right * FMath::Cos(Theta) + Up * FMath::Sin(Theta)) * Radius;
		PDI->DrawLine(PrevEquator, NextEquator, Color, SDPG_Foreground);
		PrevEquator = NextEquator;
	}

	// 자오선 8개 — 적도에서 Forward 극까지 90도 호
	constexpr int32 NumMeridians = 8;
	constexpr int32 ArcSides = NumSides / 2;
	for (int32 m = 0; m < NumMeridians; ++m)
	{
		const float Phi = 2.f * PI * m / NumMeridians;
		const FVector RadialDir = Right * FMath::Cos(Phi) + Up * FMath::Sin(Phi);

		FVector PrevArc = Origin + RadialDir * Radius;
		for (int32 i = 1; i <= ArcSides; ++i)
		{
			const float T = (PI * 0.5f) * i / ArcSides;
			const FVector NextArc = Origin + (RadialDir * FMath::Cos(T) + Forward * FMath::Sin(T)) * Radius;
			PDI->DrawLine(PrevArc, NextArc, Color, SDPG_Foreground);
			PrevArc = NextArc;
		}
	}
}

void US1AnimNotifyState_AtkCollision::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	if (nullptr == PDI || nullptr == MeshComp || ES1AtkCollisionShape::None == VirtualShape)
	{
		return;
	}

	// 무기/캐릭터 컨텍스트가 없는 프리뷰 환경이므로 항상 캐릭터 메시 소켓 기준으로 계산
	const FTransform ShapeTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, VirtualSocketName, VirtualSpawnOffset);
	DrawCoordinateSystem(PDI, ShapeTransform.GetLocation(), ShapeTransform.GetRotation().Rotator(), 20.f, SDPG_Foreground);

	switch (VirtualShape)
	{
	case ES1AtkCollisionShape::Sphere:
		DrawWireSphere(PDI, ShapeTransform, FLinearColor(0.f, 1.f, 1.f), VirtualSphereRadius, 16, SDPG_Foreground);
		break;

	case ES1AtkCollisionShape::Box:
		DrawWireBox(PDI, ShapeTransform.ToMatrixWithScale(), FBox(-VirtualBoxExtent, VirtualBoxExtent), FLinearColor(1.f, 0.5f, 0.f), SDPG_Foreground);
		break;

	case ES1AtkCollisionShape::Hemisphere:
		DrawAtkCollisionWireHemisphere(PDI, ShapeTransform, VirtualSphereRadius, FLinearColor(1.f, 0.f, 1.f));
		break;

	default:
		break;
	}
}
#endif
