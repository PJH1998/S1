// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/S1AnimNotify_SpawnLoopCycleEffect.h"
#include "Animation/S1EffectSpawnLibrary.h"
#include "Effect/NiagaraEffect/S1NiagaraEffect.h"
#include "Character/S1Character.h"
#include "Tags/S1GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#if WITH_EDITOR
#include "PrimitiveDrawingUtils.h"
#endif

US1AnimNotify_SpawnLoopCycleEffect::US1AnimNotify_SpawnLoopCycleEffect()
{
	AssetTag = S1AssetTags::Asset_Effect;
}

void US1AnimNotify_SpawnLoopCycleEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (nullptr == MeshComp)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	// 애님/몽타주 에디터 프리뷰에서만 — 무기/캐릭터 컨텍스트가 없으므로 항상 캐릭터 메시 소켓 기준, 이벤트는 안 보냄(GA 없음)
	if (::IsValid(PreviewNiagaraSystem) && nullptr != MeshComp->GetWorld() && EWorldType::EditorPreview == MeshComp->GetWorld()->WorldType)
	{
		if (bAttachToSocket)
		{
			UNiagaraComponent* PreviewComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				PreviewNiagaraSystem,
				MeshComp,
				SpawnSocketName,
				SpawnOffset.GetLocation(),
				SpawnOffset.GetRotation().Rotator(),
				SpawnOffset.GetScale3D(),
				EAttachLocation::KeepRelativeOffset,
				true,
				ENCPoolMethod::AutoRelease,
				true);

			if (nullptr != PreviewComponent)
			{
				PreviewComponent->SetAbsolute(false, false, true);
				PreviewComponent->SetVariableLinearColor(ColorParameterName, InitialColor);
			}
		}
		else
		{
			const FTransform PreviewTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, SpawnSocketName, SpawnOffset);
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
				PreviewComponent->SetVariableLinearColor(ColorParameterName, InitialColor);
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

	// 데디 서버는 렌더링 대상이 없어 스폰은 스킵 — 단, LoopCycle 이벤트는 서버 권위 사이클 카운트/자동탈출 로직을 위해 무조건 보냄
	if (Character->GetNetMode() != NM_DedicatedServer)
	{
		const TArray<TWeakObjectPtr<UNiagaraComponent>>* ExistingEffects = Character->FindAttachedEffects(EffectTag);
		if (nullptr == ExistingEffects || 0 == ExistingEffects->Num())
		{
			// 첫 사이클에만 스폰 — 이후 사이클은 GA가 같은 컴포넌트를 Activate(true)로 재트리거
			AS1NiagaraEffect* NiagaraEffectCDO = S1EffectSpawnLibrary::FindNiagaraEffectCDO(AssetTag, EffectTag);
			USkeletalMeshComponent* TargetMesh = (nullptr != NiagaraEffectCDO) ? S1EffectSpawnLibrary::ResolveTargetMesh(AttachTarget, Character, MeshComp) : nullptr;

			if (nullptr != TargetMesh)
			{
				UNiagaraComponent* Component = nullptr;
				if (bAttachToSocket)
				{
					// bAutoRelease=false — GA가 Activate(true)로 반복 재트리거하는 동안 시스템 완료(Once+Burst) 시 풀로 반납되면 안 됨
					NiagaraEffectCDO->PlayEffectAttached(TargetMesh, SpawnSocketName, SpawnOffset, &Component, false);

					if (nullptr != Component)
					{
						// Scale은 항상 절대값(부모 스케일 무시), Location/Rotation은 소켓 그대로 추종
						Component->SetAbsolute(false, false, true);
					}
				}
				else
				{
					const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(TargetMesh, SpawnSocketName, SpawnOffset);
					NiagaraEffectCDO->PlayEffect(MeshComp->GetWorld(), SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnTransform.GetScale3D(), &Component, false);
				}

				if (nullptr != Component)
				{
					Component->SetVariableLinearColor(ColorParameterName, InitialColor);
					S1EffectSpawnLibrary::ApplyCustomDepthStencil(Component, bUseCustomDepth, CustomDepthStencilValue);
					Character->RegisterAttachedEffect(EffectTag, Component);
				}
			}
		}
	}

	if (false == EventTag.IsValid())
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = Character;
	Payload.Target = Character;
	if (EffectTag.IsValid())
	{
		Payload.InstigatorTags.AddTag(EffectTag);
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, EventTag, Payload);
}

#if WITH_EDITOR
void US1AnimNotify_SpawnLoopCycleEffect::DrawInEditor(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* MeshComp, const UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent) const
{
	if (nullptr == PDI || nullptr == MeshComp)
	{
		return;
	}

	// 무기/캐릭터 컨텍스트가 없는 프리뷰 환경이므로 항상 캐릭터 메시 소켓 기준으로 계산
	const FTransform SpawnTransform = S1EffectSpawnLibrary::ComputeSpawnTransform(MeshComp, SpawnSocketName, SpawnOffset);
	DrawCoordinateSystem(PDI, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), 20.f, SDPG_Foreground);
	DrawWireSphere(PDI, SpawnTransform, FLinearColor::Yellow, 5.f, 12, SDPG_Foreground);
}
#endif
