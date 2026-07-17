// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/S1AnimNotifyState_SpawnEffect.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "Character/S1Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#if WITH_EDITOR
#include "PrimitiveDrawingUtils.h"
#endif

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
				// Scale은 항상 절대값(부모 스케일 무시), Location/Rotation은 소켓 그대로 추종
				PreviewComponent->SetAbsolute(false, false, true);
				PreviewComponent->SetVariableLinearColor(GetColorParameterName(), GetInitialColor());
			}
		}
		else
		{
			const FTransform PreviewTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, GetSpawnSocketName(), GetSpawnOffset());
			UNiagaraComponent* PreviewComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				MeshComp->GetWorld(),
				PreviewNiagaraSystem,
				PreviewTransform.GetLocation(),
				PreviewTransform.GetRotation().Rotator(),
				PreviewTransform.GetScale3D(),
				true,
				true,
				ENCPoolMethod::AutoRelease);

			if (nullptr != PreviewComponent)
			{
				PreviewComponent->SetVariableLinearColor(GetColorParameterName(), GetInitialColor());
			}
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

		// Scale은 항상 절대값(부모 스케일 무시), Location/Rotation은 소켓 그대로 추종
		if (nullptr != Component)
		{
			Component->SetAbsolute(false, false, true);
		}
	}
	else
	{
		// 소켓을 계속 따라다니지 않고, 스폰 시점의 트랜스폼을 스냅샷으로 사용
		const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(TargetMesh, GetSpawnSocketName(), GetSpawnOffset());
		NiagaraEffectCDO->PlayEffect(MeshComp->GetWorld(), SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnTransform.GetScale3D(), &Component);
	}

	if (nullptr != Component)
	{
		Component->SetVariableLinearColor(GetColorParameterName(), GetInitialColor());
		S1EffectSpawnLibrary::ApplyCustomDepthStencil(Component, ShouldUseCustomDepth(), GetCustomDepthStencilValue());
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

	Character->EndAttachedEffect(GetEffectTag());
}

#if WITH_EDITOR
void US1AnimNotifyState_SpawnEffect::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	if (nullptr == PDI || nullptr == MeshComp)
	{
		return;
	}

	// 무기/캐릭터 컨텍스트가 없는 프리뷰 환경이므로 항상 캐릭터 메시 소켓 기준으로 계산
	const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, GetSpawnSocketName(), GetSpawnOffset());
	DrawCoordinateSystem(PDI, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), 20.f, SDPG_Foreground);
	DrawWireSphere(PDI, SpawnTransform, FLinearColor::Yellow, 5.f, 12, SDPG_Foreground);
}
#endif
