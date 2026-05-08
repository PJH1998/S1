// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Cursor/S1Cursor.h"

US1Cursor::US1Cursor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US1Cursor::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
}
