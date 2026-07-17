#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "S1Enums.h"
#include "S1SelectCharacter.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class AS1Weapon;
class US1WeaponAnimLayer;
class US1CharacterSelectData;

UCLASS()
class S1_API AS1SelectCharacter : public APawn
{
	GENERATED_BODY()

public:
	AS1SelectCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void ChangeMesh(const FGameplayTag& EntryTag);
	void ChangeWeapon(const FGameplayTag& WeaponTag);

	void SetOrbitEnabled(bool bEnabled);
	void AddCameraYaw(float DeltaYaw);
	void AddCameraPitch(float DeltaPitch);

	void StopPreviewCapture();

	UTextureRenderTarget2D* GetPreviewRenderTarget() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FaceMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneCaptureComponent2D> PreviewCapture;

	// 크기/포맷 참조용 템플릿 — 실제로는 이 값을 그대로 쓰지 않고 인스턴스별로 복제해서 사용(아래 InstanceRenderTarget)
	UPROPERTY(EditDefaultsOnly, Category = "Preview")
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	// BeginPlay에서 PreviewRenderTarget을 복제해 생성 — 이게 없으면 멀티 클라에서 여러 SelectCharacter 인스턴스가
	// 같은 RT 에셋에 매 프레임 캡처를 덮어써 서로의 프리뷰가 뒤섞임(1인 PIE에서만 정상 동작하던 원인)
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> InstanceRenderTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<US1CharacterSelectData> CharacterSelectData;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("w_R_Attach");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName OffhandSocketName = FName("w_L_Attach");

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float OrbitSpeed = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector2D PitchClamp = FVector2D(-70.f, 20.f);

private:
	UPROPERTY()
	TObjectPtr<AS1Weapon> EquippedWeapon;

	UPROPERTY()
	TObjectPtr<AS1Weapon> EquippedOffhandWeapon;

	TSubclassOf<US1WeaponAnimLayer> CurrentAnimLayerClass;

	EPlayerGender CurrentGender = EPlayerGender::Female;

	bool bOrbitEnabled = false;
};
