// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define D(x) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, x); }

namespace S1Levels
{
	const FName LoadingMap = TEXT("LoadingMap");
}