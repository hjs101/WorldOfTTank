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
	ASunnyGameMode();

	void ActorDied(AActor* DeadActor);

	UPROPERTY(EditAnywhere, Category = "MiniMap")
	class USceneCaptureComponent2D* SceneCaptureComp;

protected:

	virtual void BeginPlay() override;

	


private:

	//class ASunnyTTank* TTank;
	//class ASunnyPlayerController* TTanksPlayerController;
	
};
