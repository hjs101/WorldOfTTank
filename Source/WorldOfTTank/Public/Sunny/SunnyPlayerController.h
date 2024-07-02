// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SunnyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	// 플레이어 활성/비활성
	void SetPlayerEnabledState(bool bPlaerEnabled);
	
};
