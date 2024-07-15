// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HJS/RankingStruct.h"
#include "GameFramework/GameModeBase.h"
#include "SunnyGameMode.generated.h"

/**
 * 
 */

struct FRankingData;

UCLASS()
class WORLDOFTTANK_API ASunnyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASunnyGameMode();

	//void ActorDied(AActor* DeadActor);

	void OnEnemyDie();

	void RestartGame();

	void QuitGame();

	void UpdateRanking(FString Name, float Playtime);
	
	void LoadRankingData();

	void EndGame();

	void ConvertRankingToString();
protected:

	virtual void BeginPlay() override;
	


private:

	//class ASunnyTTank* TTank;
	//class ASunnyPlayerController* TTanksPlayerController;


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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEndGameWidget> EndGameUIFactory;

	UPROPERTY()
	class UEndGameWidget* EndGameUI;

	UFUNCTION()
	void VolumeDown();

	UPROPERTY(EditAnywhere)
	float BGMRate = 15.f;


	// 미니맵 카메라
	class USceneCaptureComponent2D* MiniMapCamera;
	
	UPROPERTY(EditAnywhere, Category = "Ranking")
	class UDataTable* RankingDataTable;

	TArray<FRankingData> Ranking;
	
	bool bVictory;

	float StartTime;
	float AccPlayTime;
};