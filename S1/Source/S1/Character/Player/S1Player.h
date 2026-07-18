// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/S1Character.h"
#include "Data/S1InputData.h"
#include "GameplayPrediction.h"
#include "S1Enums.h"
#include "S1Player.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class USphereComponent;
class UBoxComponent;
class AS1PlayerController;
class AS1Weapon;
class US1LockOnComponent;
class US1InteractComponent;
class AS1PuzzleButton_Gimmick;
class US1PlayerCameraComponent;
class US1PlayerReactBridgeComponent;
class US1WeaponAnimLayer;
class FLifetimeProperty;
class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;
class ACineCameraActor;
class UInputComponent;

UCLASS()
class S1_API AS1Player : public AS1Character
{
	GENERATED_BODY()
	
public:
	AS1Player();

protected:
	virtual void	BeginPlay() override;
	virtual void	PossessedBy(AController* NewController) override;
	virtual void	OnRep_PlayerState() override;
	virtual void	Landed(const FHitResult& Hit) override;
	// 공중 상태 태그 관리 — CMC 이동모드 기준(클라 예측/서버 일관) → 지상에서 공중공격 발동 방지
	virtual void	OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void	SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void	Tick(float DeltaTime) override;
	virtual void	Jump() override;
	virtual void	AddMovementInput(FVector WorldDirection, float ScaleValue = 1.f, bool bForce = false) override;
	virtual void	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void	InitSystem() override;

	void			    ActivateAbility(const FGameplayTag& AbilityTag);
	void			    ReleaseAbility(const FGameplayTag& AbilityTag);
	void			    SetSprinting(bool bInSprint);
	bool			    GetSprinting();

	// InteractComponent가 찾은 가장 가까운 버튼을 눌러본다(대상 없으면 아무 일도 안 함).
	void			    TryInteract();

private:
	// 입력 → 서버 라우팅 (ServerOnly GA — 서버에서 활성화/콤보/크로스 전부 처리)
	// bInAir: 입력 시점 클라의 공중 상태 — 서버 이동상태 랙 보정용 (지상/공중 공격 정확 판정)
	UFUNCTION(Server, Reliable)
	void ServerActivateAbility(const FGameplayTag& AbilityTag, bool bInAir);

	// 예측 콤보 어드밴스 — 클라가 생성한 예측키로 서버가 같은 재활성화를 미러 (루트모션 공유 키)
	UFUNCTION(Server, Reliable)
	void ServerReactivateAbility(const FGameplayTag& AbilityTag, FPredictionKey Key);

	// 키 업 — 서버 GA에 OnInputReleased 전달
	UFUNCTION(Server, Reliable)
	void ServerReleaseAbility(const FGameplayTag& AbilityTag);

	// 점프 시 GAS 처리(이벤트 발송 + 공중 태그)를 서버에도 반영
	UFUNCTION(Server, Reliable)
	void ServerNotifyJump();

	// Interact 입력 → 서버 라우팅. 서버에서 거리 재검증 후 Target->PressButton() 호출.
	UFUNCTION(Server, Reliable)
	void ServerInteract(AS1PuzzleButton_Gimmick* Target);

	// 점프 GAS 처리 본체 — 소유 클라/서버 각 머신에서 로컬 실행
	void HandleJumpGAS();

	// 공중 상태 적용 — true면 State.Air 부여, false면 State.Air 하위 전부 + State.Used.Air 하위 전부 제거
	// (하위 태그가 남으면 지상에서도 공중공격으로 오판정되거나, 공중 스킬 1회 제한 태그가 안 풀림)
	void ApplyAirState(bool bInAir);

	// 스프린트 상태를 서버에 전달 (이동속도 + 복제 bSprint 갱신)
	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bInSprint);

	// 디버그 전용 — NumPad 7/8/9로 GA_Hit_Weak/Strong/ToAir 강제 트리거 (공격원 없이 원점(0,0,0) 기준 방향)
	// UFUNCTION은 UHT 제약상 헤더에서 #if(WITH_EDITORONLY_DATA 제외)로 못 감쌈 — 실제 스트리핑은 .cpp에서 처리
	void DebugTriggerHit_Weak();
	void DebugTriggerHit_Strong();
	void DebugTriggerHit_ToAir();

	UFUNCTION(Server, Reliable)
	void ServerDebugTriggerHit(FGameplayTag HitTypeTag);

	// 디버그 전용 — 키보드 0(넘패드 아님)으로 얼티밋 게이지 즉시 Max
	void DebugSetUltimateGaugeMax();

	UFUNCTION(Server, Reliable)
	void ServerDebugSetUltimateGaugeMax();

public:
	AS1Weapon*		    GetEquippedWeapon()        const { return EquippedWeapon; }
	AS1Weapon*		    GetEquippedOffhandWeapon() const { return EquippedOffhandWeapon; }
	US1LockOnComponent* GetLockOnComponent()   const { return LockOnComponent; }
	EPlayerGender       GetGender()             const { return Gender; }
	const TArray<FS1AbilityInputBinding>& GetAbilityInputBindings() const { return AbilityInputBindings; }

	// AtkCollision 노티파이의 VirtualShape가 켜졌을 때 GA_Attack이 켜고 끄는 가상 판정 도형 — Main/Offhand 공용 1개씩
	USphereComponent* GetVirtualSphereCollision() const { return VirtualSphereCollision; }
	UBoxComponent*    GetVirtualBoxCollision()    const { return VirtualBoxCollision; }

	UFUNCTION()
	void OnItemEquipped(FGameplayTag ItemTag);

	// Ultimate 컷씬 — 시전 클라에서만 시퀀스+카메라 프레젠테이션 (서버 GA가 호출)
	UFUNCTION(Client, Reliable)
	void ClientPlayUltimateCutscene(ULevelSequence* Sequence, FName InPlayerBindingTag, float InEnterBlendTime, float InExitBlendTime);

	// Ultimate 취소 시 컷씬 즉시 중단 + 카메라 복귀
	UFUNCTION(Client, Reliable)
	void ClientStopUltimateCutscene();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> FaceMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	EPlayerGender Gender = EPlayerGender::Female;

	// 무기 없을 때(맨손) 사용할 WeaponData 엔트리 키 — WeaponClass/WeaponAbilitiesTag/AnimLayer 전부 이 태그로 조회
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Default")
	FGameplayTag DefaultWeaponTag;

	// 무기 무관 공용 어빌리티(GA_Hit 등) — 무기 교체(EquipWeapon)와 별개로 PossessedBy에서 한 번만 부여, 이후 유지
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag DefaultAbilitiesTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	TObjectPtr<AS1Weapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Weapon")
	TObjectPtr<AS1Weapon> EquippedOffhandWeapon;

	// 장착 아이템 태그 복제 — 각 클라가 OnRep에서 AnimLayer 링크 (비주얼 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_EquippedItemTag)
	FGameplayTag EquippedItemTag;

	UFUNCTION()
	void OnRep_EquippedItemTag();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName = FName("w_R_Attach");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName OffhandSocketName = FName("w_L_Attach");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float WeaponLayerBlendTime = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<FS1AbilityInputBinding> AbilityInputBindings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1LockOnComponent> LockOnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1InteractComponent> InteractComponent;

	// 서버 거리 재검증용 — InteractSphere 반경과 별개로 여유를 둔 허용 오차.
	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractValidationRadius = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1PlayerReactBridgeComponent> ReactBridgeComponent;

	// AtkCollision 노티파이의 가상 충돌체(구/반구용) — 기본 NoCollision, GA_Attack이 히트 윈도우에서만 켬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VirtualCollision")
	TObjectPtr<USphereComponent> VirtualSphereCollision;

	// AtkCollision 노티파이의 가상 충돌체(박스용) — 기본 NoCollision, GA_Attack이 히트 윈도우에서만 켬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VirtualCollision")
	TObjectPtr<UBoxComponent> VirtualBoxCollision;

	// 가상 충돌체 콜리전 프로파일 — AS1Weapon::AttackBox와 동일하게 몬스터만 Overlap하는 무기 채널(기본 "PlayerWeapon")
	UPROPERTY(EditDefaultsOnly, Category = "VirtualCollision")
	FName VirtualCollisionProfileName = TEXT("PlayerWeapon");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<US1PlayerCameraComponent> Camera;

private:
	// 현재 장착된 무기의 WeaponType — 같은 타입 재장착 시 AnimLayer 교체 건너뜀
	ES1WeaponType CurrentWeaponType = ES1WeaponType::None;

	// 현재 부여 중인 무기 GA 그룹 태그 (회수 시 사용)
	FGameplayTag CurrentWeaponAbilitiesTag;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<AS1PlayerController> PlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeed = 1000.f;

	// 소유 클라는 로컬 예측값 유지(COND_SkipOwner), 타 클라/서버 애니메이션용 복제
	UPROPERTY(Replicated)
	bool bSprint = false;

	// 액션 중 점프 차단 조건 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag ActionStateTag;

	// 공중 상태 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag AirStateTag;

	// 공중 스킬 1회 제한용 상위 태그(State.Used.Air.*) — AirStateTag(State.Air) 하위가 아닌 별도 트리라
	// 착지 시 ApplyAirState(false)에서 AirStateTag와 별개로 이 트리도 같이 제거해야 함
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag UsedAirStateTag;

	void EquipWeapon(const FGameplayTag& ItemTag);

	// 클라에서 장착 호출 시 서버로 전달 (테스트 키 / 외부 요청 공통)
	UFUNCTION(Server, Reliable)
	void ServerRequestEquip(const FGameplayTag& ItemTag);

	// 무기 AnimLayer 링크 + Inertialization (서버/각 클라 비주얼 공통)
	void LinkWeaponAnimLayer(TSubclassOf<US1WeaponAnimLayer> AnimLayerClass);

	// BaseTag에 성별 접미사를 붙여 반환 (e.g. "Asset.Ability.Player.SWD" → "Asset.Ability.Player.SWD.Female")
	FGameplayTag AppendGenderSuffix(const FGameplayTag& BaseTag) const;

	// Jump() 호출 시 GA에 전달할 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag JumpEventTag;

public:
	// GA_Action::OnMoveBeginReceived에서 S키(반대 방향) 체크용 (서버 GA가 읽음)
	FVector GetLastInputDirection() const { return LastInputDirection; }
	// OnMove(Triggered/Completed)에서 설정 — 로컬 적용 + 변경 시 서버로 전달
	void SetLastInputDirection(const FVector& InDirection);

private:
	// 원시 입력 방향을 서버에 전달 (State.Action 중엔 CMC 벡터가 0이라 별도 동기화 필요)
	UFUNCTION(Server, Unreliable)
	void ServerSetLastInputDirection(const FVector& InDirection);

	// 마지막으로 서버에 보낸 방향 — 의미있는 변화 시에만 재전송 (대역폭 절약)
	FVector LastSentInputDirection = FVector::ZeroVector;

public:

private:
	// 현재 이동 입력 방향 (키 없으면 ZeroVector, OnMove Completed에서 리셋)
	FVector LastInputDirection = FVector::ZeroVector;

	// Landed() 호출 시 GA에 전달할 이벤트 태그
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTag LandedEventTag;

	// ── Ultimate 컷씬 프레젠테이션 (시전 클라 로컬) ──────────────────────
	void StartUltimateCutscene();
	void UltiTryBlendToCineCamera();
	ACineCameraActor* UltiFindSequenceCineCamera() const;
	void UltiStartExitBlend();
	UFUNCTION() void OnUltiCutsceneFinished();
	void UltiCleanup();

	// 컷씬 "플레이어만 보이기" PP가 시전자만 골라내도록, 시전 중엔 메시 스텐실을 UltimateSpotlight로 임시 전환
	// (평소엔 다른 플레이어와 동일한 Player 값이라 멀티에서 구분 불가 — §S1Enums.h ES1StencilLayer 참조)
	void SetUltimateStencilActive(bool bActive);

	UPROPERTY(Transient) TObjectPtr<ULevelSequence>       UltiSequence;
	UPROPERTY(Transient) TObjectPtr<ULevelSequencePlayer> UltiSequencePlayer;
	UPROPERTY(Transient) TObjectPtr<ALevelSequenceActor>  UltiSequenceActor;
	UPROPERTY(Transient) TObjectPtr<ACineCameraActor>     UltiCineCamera;

	FName  UltiPlayerBindingTag    = TEXT("PlayerAnchor");
	float  UltiEnterBlendTime      = 0.3f;
	float  UltiExitBlendTime       = 0.5f;
	bool   bUltiCameraBlendRetried = false;
	bool   bUltiExitBlendStarted   = false;

	FTimerHandle UltiExitStartTimer;
	FTimerHandle UltiCleanupTimer;
};
