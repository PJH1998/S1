// Fill out your copyright notice in the Description page of Project Settings.

#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void AS1NiagaraEffect::PlayEffect(UWorld* World, FVector Location, FRotator Rotation, FVector Scale, UNiagaraComponent** OutComponent, bool bAutoRelease) const
{
	if (nullptr == World || nullptr == NiagaraSystem)
	{
		return;
	}

	UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		Location,
		Rotation,
		Scale,
		bAutoRelease,	// bAutoDestroy — false면 시스템 완료돼도 파괴 안 되고 호출부가 수명을 직접 관리(Activate(true) 재사용 등)
		true,
		bAutoRelease ? ENCPoolMethod::AutoRelease : ENCPoolMethod::None);

	if (nullptr != OutComponent)
	{
		*OutComponent = Component;
	}
}

void AS1NiagaraEffect::PlayEffectAttached(USceneComponent* AttachToComponent, FName SocketName, const FTransform& SpawnOffset, UNiagaraComponent** OutComponent, bool bAutoRelease) const
{
	if (nullptr == AttachToComponent || nullptr == NiagaraSystem)
	{
		return;
	}

	// Scale/Rotation 절대값 처리는 호출부(NotifyState)의 옵션에 따라 달라져서 여기서는 순수 스폰만 담당
	UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		AttachToComponent,
		SocketName,
		SpawnOffset.GetLocation(),
		SpawnOffset.GetRotation().Rotator(),
		SpawnOffset.GetScale3D(),
		EAttachLocation::KeepRelativeOffset,
		bAutoRelease,	// bAutoDestroy — false면 시스템 완료돼도 파괴 안 되고 호출부가 수명을 직접 관리(Activate(true) 재사용 등)
		bAutoRelease ? ENCPoolMethod::AutoRelease : ENCPoolMethod::None,
		true);

	if (nullptr != OutComponent)
	{
		*OutComponent = Component;
	}
}
