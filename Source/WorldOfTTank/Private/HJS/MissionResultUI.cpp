// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/MissionResultUI.h"

void UMissionResultUI::PlayAnim()
{
	if (FadeIn)
	{
		PlayAnimation(FadeIn);
	}
}

void UMissionResultUI::NativeConstruct()
{
	Super::NativeConstruct();
}
