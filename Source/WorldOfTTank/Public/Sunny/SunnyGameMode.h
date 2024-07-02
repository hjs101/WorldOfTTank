// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SunnyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	void ActorDied(AActor* DeadActor);

protected:

	virtual void BeginPlay() override;


private:

	class ASunnyTTank* TTank;
	class ASunnyPlayerController* TTanksPlayerController;
	
};
