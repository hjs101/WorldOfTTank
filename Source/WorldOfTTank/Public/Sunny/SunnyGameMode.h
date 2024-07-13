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

	void OnEnemyDie();

protected:

	virtual void BeginPlay() override;


private:

	class ASunnyTTank* TTank;
	class ASunnyPlayerController* TTanksPlayerController;
	UPROPERTY(EditAnywhere,Category="SFX")
	class USoundBase* BGM;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UAudioComponent* BGMComponent;

	FTimerHandle BGMTimerHandle;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTotalState> TotalStateUIFactory;

	int32 EnemyCount = 2;

	UPROPERTY()
	class UTotalState* TotalStateUI;

	UFUNCTION()
	void VolumeDown();

	UPROPERTY(EditAnywhere)
	float BGMRate = 15.f;
	
};
