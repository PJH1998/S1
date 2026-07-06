// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffect.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "Character/S1Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void US1AnimNotifyState_SpawnEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	// 애님/몽타주 에디터 프리뷰에서만 — 무기/캐릭터 컨텍스트가 없으므로 항상 캐릭터 메시 소켓 기준
	if (::IsValid(PreviewNiagaraSystem) && IsEditorPreviewWorld(MeshComp->GetWorld()))
	{
		if (bAttachToSocket)
		{
			UNiagaraComponent* PreviewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				PreviewNiagaraSystem,
				MeshComp,
				GetSpawnSocketName(),
				GetSpawnOffset().GetLocation(),
				GetSpawnOffset().GetRotation().Rotator(),
				GetSpawnOffset().GetScale3D(),
				EAttachLocation::KeepRelativeOffset,
				true,
				ENCPoolMethod::AutoRelease,
				true);

			if (nullptr != PreviewComponent)
			{
				PreviewComponent->SetAbsolute(false, !bFollowSocketRotation, true);
			}
		}
		else
		{
			const FTransform PreviewTransform = GetSpawnOffset() * MeshComp->GetSocketTransform(GetSpawnSocketName());
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				MeshComp->GetWorld(),
				PreviewNiagaraSystem,
				PreviewTransform.GetLocation(),
				PreviewTransform.GetRotation().Rotator(),
				PreviewTransform.GetScale3D(),
				true,
				true,
				ENCPoolMethod::AutoRelease);
		}
		return;
	}
#endif

	AS1Character* Character = Cast<AS1Character>(MeshComp->GetOwner());
	if (nullptr == Character)
	{
		return;
	}

	// 데디 서버는 렌더링 대상이 없어 스폰 자체가 낭비 — 스킵 (몽타주가 서버에서도 재생되어 NotifyBegin은 발화함)
	if (Character->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	AS1NiagaraEffect* NiagaraEffectCDO = FindNiagaraEffectCDO();
	if (nullptr == NiagaraEffectCDO)
	{
		return;
	}

	USkeletalMeshComponent* TargetMesh = ResolveTargetMesh(Character, MeshComp);
	if (nullptr == TargetMesh)
	{
		return;
	}

	UNiagaraComponent* Component = nullptr;
	if (bAttachToSocket)
	{
		NiagaraEffectCDO->PlayEffectAttached(TargetMesh, GetSpawnSocketName(), GetSpawnOffset(), &Component);

		// Scale은 항상 절대값, Rotation은 bFollowSocketRotation 옵션에 따라 소켓 추종 여부 결정
		if (nullptr != Component)
		{
			Component->SetAbsolute(false, !bFollowSocketRotation, true);
		}
	}
	else
	{
		// 소켓을 계속 따라다니지 않고, 스폰 시점의 소켓 트랜스폼에 SpawnOffset을 곱한 결과를 스냅샷으로 사용
		const FTransform SpawnTransform = GetSpawnOffset() * TargetMesh->GetSocketTransform(GetSpawnSocketName());
		NiagaraEffectCDO->PlayEffect(MeshComp->GetWorld(), SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnTransform.GetScale3D(), &Component);
	}

	Character->RegisterAttachedEffect(GetEffectTag(), Component);
}

void US1AnimNotifyState_SpawnEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

	AS1Character* Character = Cast<AS1Character>(MeshComp->GetOwner());
	if (nullptr == Character)
	{
		return;
	}

	if (bAttachToSocket)
	{
		Character->DetachAttachedEffect(GetEffectTag());
	}
	else
	{
		Character->RemoveAttachedEffect(GetEffectTag());
	}
}
