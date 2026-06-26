// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/Decal/S1Decal_AttackRange.h"
#include "Materials/MaterialInstanceDynamic.h"

void AS1Decal_AttackRange::ShowAttackRange(const FS1AttackRangeDecalRequest& InRequest)
{
	Request = InRequest;
	ElapsedTime = 0.f;

	ShowDecal(Request.Location, Request.Rotation, CalculateDecalSize());
	ApplyMaterialParameters(0.f);
}

bool AS1Decal_AttackRange::UpdateDecal(float DeltaTime)
{
	if (false == IsActive())
	{
		return true;
	}

	// 머티리얼 준비 전까진 노출/타이머 진행을 보류한다(콜드 캐시 첫 렌더 깜빡임 방지).
	if (false == TryReveal())
	{
		return false;
	}

	if (Request.Duration <= 0.f)
	{
		ApplyMaterialParameters(1.f);
		return false;
	}

	ElapsedTime += DeltaTime;

	const float Progress = FMath::Clamp(ElapsedTime / Request.Duration, 0.f, 1.f);
	ApplyMaterialParameters(Progress);

	return Progress >= 1.f;
}

void AS1Decal_AttackRange::HideDecal()
{
	ElapsedTime = 0.f;
	Super::HideDecal();
}

FVector AS1Decal_AttackRange::CalculateDecalSize() const
{
	const float ProjectionDepth = FMath::Max(Request.ProjectionDepth, 1.f);

	switch (Request.Shape)
	{
	case ES1AttackRangeDecalShape::Rectangle:
		return FVector(ProjectionDepth, FMath::Max(Request.Width, 1.f), FMath::Max(Request.Length, 1.f));

	case ES1AttackRangeDecalShape::Cone:
	case ES1AttackRangeDecalShape::Circle:
	default:
	{
		const float Diameter = FMath::Max(Request.Radius * 2.f, 1.f);
		return FVector(ProjectionDepth, Diameter, Diameter);
	}
	}
}

void AS1Decal_AttackRange::ApplyMaterialParameters(float Progress)
{
	UMaterialInstanceDynamic* MID = CreateAndApplyDynamicMaterial();
	if (MID == nullptr)
	{
		return;
	}

	MID->SetScalarParameterValue(TEXT("ShapeType"), static_cast<float>(Request.Shape));
	MID->SetScalarParameterValue(TEXT("Radius"), Request.Radius);
	MID->SetScalarParameterValue(TEXT("Angle"), Request.Angle);
	MID->SetScalarParameterValue(TEXT("Width"), Request.Width);
	MID->SetScalarParameterValue(TEXT("Length"), Request.Length);
	MID->SetScalarParameterValue(TEXT("Progress"), Progress);
	MID->SetScalarParameterValue(TEXT("Opacity"), Request.Color.A);
	MID->SetVectorParameterValue(TEXT("Color"), Request.Color);

	const FRotator YawRotation(0.f, Request.Rotation.Yaw, 0.f);
	MID->SetVectorParameterValue(TEXT("DecalOrigin"), FLinearColor(Request.Location));
	MID->SetVectorParameterValue(TEXT("DecalForward"), FLinearColor(YawRotation.Vector()));
	MID->SetVectorParameterValue(TEXT("DecalRight"), FLinearColor(FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y)));
}
