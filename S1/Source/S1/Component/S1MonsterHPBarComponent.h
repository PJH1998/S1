// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "S1MonsterHPBarComponent.generated.h"

class AS1Monster;
class US1MonsterHPBar;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class S1_API US1MonsterHPBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	US1MonsterHPBarComponent();

	void ResetForPoolSpawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HPBar", meta = (AllowPrivateAccess = "true"))
	FName AttachHPBarSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HPBar", meta = (AllowPrivateAccess = "true"))
	FVector HPBarRelativeLocation = FVector(0.f, 0.f, 20.f);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ApplyWidgetSettings();
	void InitializeWidget();
	void UpdateBarVisibility(bool bInHasTarget);

	UFUNCTION()
	void HandleHasTargetChanged(AS1Monster* InMonster, bool bInHasTarget);

	UFUNCTION()
	void HandleMonsterNameChanged(AS1Monster* InMonster);

	UFUNCTION()
	void HandleReturnedToPool(AS1Monster* InMonster);

	UFUNCTION()
	void HandleHideAnimationFinished();

private:
	UPROPERTY(Transient)
	TObjectPtr<AS1Monster> OwnerMonster;

	UPROPERTY(Transient)
	TObjectPtr<US1MonsterHPBar> MonsterHPBar;
};
