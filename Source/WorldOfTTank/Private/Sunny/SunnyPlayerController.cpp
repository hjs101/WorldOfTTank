// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyPlayerController.h"
#include "Sunny/SunnyBasePawn.h"


void  ASunnyPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
	if (bPlayerEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}

	bShowMouseCursor = bPlayerEnabled;
}

