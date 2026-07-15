#include "Character/S1SelectCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Data/S1CharacterSelectData.h"
#include "Data/S1WeaponData.h"
#include "Weapon/S1Weapon.h"
#include "Animation/Weapon/S1WeaponAnimLayer.h"
#include "System/S1AssetManager.h"
#include "Tags/S1GameplayTags.h"
#include "S1LogChannels.h"

AS1SelectCharacter::AS1SelectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);

	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(BodyMesh);
	HairMesh->SetLeaderPoseComponent(BodyMesh);

	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FaceMesh"));
	FaceMesh->SetupAttachment(BodyMesh);
	FaceMesh->SetLeaderPoseComponent(BodyMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 90.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	PreviewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("PreviewCapture"));
	PreviewCapture->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PreviewCapture->bCaptureEveryFrame = true;
	PreviewCapture->bCaptureOnMovement = false;
	PreviewCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	PreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
}

void AS1SelectCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (::IsValid(PreviewCapture))
	{
		// 멀티 클라에서 SelectCharacter 인스턴스마다 RT를 따로 가지도록 템플릿을 복제 — 공유 에셋을 그대로 쓰면
		// 여러 인스턴스의 캡처가 같은 RT를 매 프레임 덮어써서 서로의 프리뷰가 뒤섞임
		if (::IsValid(PreviewRenderTarget))
		{
			// DuplicateObject는 GPU 리소스(RHI 텍스처)를 새로 안 만들어 검은 화면이 나옴 —
			// CreateRenderTarget2D로 템플릿과 같은 크기/포맷의 새 RT를 제대로 초기화해서 생성
			InstanceRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
				this,
				PreviewRenderTarget->SizeX,
				PreviewRenderTarget->SizeY,
				PreviewRenderTarget->RenderTargetFormat,
				PreviewRenderTarget->ClearColor,
				PreviewRenderTarget->bAutoGenerateMips);
		}

		PreviewCapture->TextureTarget = GetPreviewRenderTarget();
		PreviewCapture->ShowOnlyActors.Reset();
		PreviewCapture->ShowOnlyActors.Add(this);
	}
}

UTextureRenderTarget2D* AS1SelectCharacter::GetPreviewRenderTarget() const
{
	return ::IsValid(InstanceRenderTarget) ? InstanceRenderTarget.Get() : PreviewRenderTarget.Get();
}

void AS1SelectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (false == bOrbitEnabled)
	{
		return;
	}

	FRotator Rot = SpringArm->GetRelativeRotation();
	Rot.Yaw += OrbitSpeed * DeltaTime;
	SpringArm->SetRelativeRotation(Rot);
}

void AS1SelectCharacter::ChangeMesh(const FGameplayTag& EntryTag)
{
	if (false == ::IsValid(CharacterSelectData))
	{
		LOG_WARNING(TEXT("ChangeMesh: CharacterSelectData is null"));
		return;
	}

	const FS1SelectCharacterEntry* Entry = CharacterSelectData->FindEntryByTag(EntryTag);
	if (nullptr == Entry)
	{
		LOG_WARNING(TEXT("ChangeMesh: no entry for tag [%s]"), *EntryTag.ToString());
		return;
	}

	CurrentGender = Entry->Gender;
	BodyMesh->SetSkeletalMeshAsset(Entry->BodyMesh);
	HairMesh->SetSkeletalMeshAsset(Entry->HairMesh);
	FaceMesh->SetSkeletalMeshAsset(Entry->FaceMesh);

	ChangeWeapon(Entry->DefaultWeaponTag);
}

void AS1SelectCharacter::ChangeWeapon(const FGameplayTag& WeaponTag)
{
	if (::IsValid(EquippedWeapon))
	{
		if (::IsValid(PreviewCapture))
		{
			PreviewCapture->ShowOnlyActors.Remove(EquippedWeapon);
		}
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	if (::IsValid(EquippedOffhandWeapon))
	{
		if (::IsValid(PreviewCapture))
		{
			PreviewCapture->ShowOnlyActors.Remove(EquippedOffhandWeapon);
		}
		EquippedOffhandWeapon->Destroy();
		EquippedOffhandWeapon = nullptr;
	}

	if (nullptr != CurrentAnimLayerClass)
	{
		BodyMesh->UnlinkAnimClassLayers(CurrentAnimLayerClass);
		CurrentAnimLayerClass = nullptr;
	}

	if (false == WeaponTag.IsValid())
	{
		return;
	}

	US1WeaponData* WeaponData = US1AssetManager::GetAssetByTag<US1WeaponData>(S1AssetTags::Asset_WeaponData);
	if (false == ::IsValid(WeaponData))
	{
		LOG_WARNING(TEXT("ChangeWeapon: WeaponData not loaded"));
		return;
	}

	const FS1WeaponEntry* Entry = WeaponData->FindEntryByTag(WeaponTag);
	if (nullptr == Entry)
	{
		LOG_WARNING(TEXT("ChangeWeapon: no entry for tag [%s]"), *WeaponTag.ToString());
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;

	if (nullptr != Entry->WeaponClass)
	{
		EquippedWeapon = GetWorld()->SpawnActor<AS1Weapon>(Entry->WeaponClass, Params);
		if (::IsValid(EquippedWeapon))
		{
			EquippedWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
			EquippedWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));

			if (::IsValid(PreviewCapture))
			{
				PreviewCapture->ShowOnlyActors.AddUnique(EquippedWeapon);
			}
		}
	}

	TSubclassOf<US1WeaponAnimLayer> LayerClass = Entry->GetAnimLayerClass(CurrentGender);
	if (nullptr == LayerClass)
	{
		LOG_WARNING(TEXT("ChangeWeapon: [%s] has no AnimLayerClass"), *WeaponTag.ToString());
	}
	else
	{
		BodyMesh->LinkAnimClassLayers(LayerClass);
		CurrentAnimLayerClass = LayerClass;
	}

	if (nullptr != Entry->OffhandWeaponClass)
	{
		EquippedOffhandWeapon = GetWorld()->SpawnActor<AS1Weapon>(Entry->OffhandWeaponClass, Params);
		if (::IsValid(EquippedOffhandWeapon))
		{
			EquippedOffhandWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, OffhandSocketName);
			EquippedOffhandWeapon->SetActorRelativeRotation(FRotator(0.f, 0.f, -90.f));

			if (::IsValid(PreviewCapture))
			{
				PreviewCapture->ShowOnlyActors.AddUnique(EquippedOffhandWeapon);
			}
		}
	}
}

void AS1SelectCharacter::SetOrbitEnabled(bool bEnabled)
{
	bOrbitEnabled = bEnabled;
}

void AS1SelectCharacter::AddCameraYaw(float DeltaYaw)
{
	FRotator Rot = SpringArm->GetRelativeRotation();
	Rot.Yaw += DeltaYaw;
	SpringArm->SetRelativeRotation(Rot);
}

void AS1SelectCharacter::AddCameraPitch(float DeltaPitch)
{
	FRotator Rot = SpringArm->GetRelativeRotation();
	Rot.Pitch = FMath::Clamp(Rot.Pitch + DeltaPitch, PitchClamp.X, PitchClamp.Y);
	SpringArm->SetRelativeRotation(Rot);
}
