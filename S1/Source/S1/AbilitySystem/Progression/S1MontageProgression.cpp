// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Progression/S1MontageProgression.h"
#include "AbilitySystem/Abilities/Player/Action/S1GameplayAbility_Action.h"

void US1MontageProgression::Init(US1GameplayAbility_Action* InGA)
{
	// GA가 활성화마다 Init → OnActivated 순으로 부른다. 여기서 세대를 올려두면
	// OnActivated가 거는 콜백은 새 번호를, 이전 활성화의 콜백은 옛 번호를 들고 있다
	++ActivationEpoch;

	GA = InGA;
}
