// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/MissionResultUI.h"

void UMissionResultUI::PlayAnim()
{
}

void UMissionResultUI::NativeConstruct()
{
	if (FadeIn)
	{
		PlayAnimation(FadeIn);
	}
}
