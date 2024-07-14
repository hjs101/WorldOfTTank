// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyGameMode.h"
//#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyNewTTank.h"
#include "Sunny/SunnyPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

#include "HJS/TotalState.h"
#include "HJS/EndGameWidget.h"




ASunnyGameMode::ASunnyGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// 미니맵 카메라 추가
	MiniMapCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCamera"));
}

void ASunnyGameMode::OnEnemyDie()
{
	TotalStateUI->UpdateTotalStateUI(--EnemyCount);
}


void ASunnyGameMode::BeginPlay()
{
	Super::BeginPlay();

	//TTank = Cast<ASunnyTTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	//TTanksPlayerController = Cast<ASunnyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (BGM)
	{
		BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BGM);
		GetWorldTimerManager().SetTimer(BGMTimerHandle, this, &ASunnyGameMode::VolumeDown, BGMRate, false);
	}

	if (TotalStateUIFactory != nullptr)
	{
		TotalStateUI = Cast<UTotalState>(CreateWidget(GetWorld(), TotalStateUIFactory, FName("TotalStateUI")));
		if (TotalStateUI != nullptr)
		{
			//화면에 보이게 
			TotalStateUI->AddToViewport();
			TotalStateUI->UpdateTotalStateUI(2);
		}
	}

	if (EndGameUIFactory != nullptr)
	{
		EndGameUI = Cast<UEndGameWidget>(CreateWidget(GetWorld(), EndGameUIFactory, FName("EndGameUI")));
		if (TotalStateUI != nullptr)
		{
			//화면에 보이게 
			TotalStateUI->AddToViewport();
		}
	}

}

void ASunnyGameMode::VolumeDown()
{
	if (BGMComponent)
	{
		BGMComponent->SetVolumeMultiplier(0.3f);
	}
}



//void ASunnyGameMode::ActorDied(AActor* DeadActor)
//{
	/*if (DeadActor == TTank)
	{
		TTank->HandleDestruction();
		if (TTanksPlayerController)
		{
			TTanksPlayerController->SetPlayerEnabledState(false);
		}
	}
	else if (ASunnyEnemy* DestroyedEnemy = Cast<ASunnyEnemy>(DeadActor))
	{
		DestroyedEnemy->HandleDestruction();
	}*/
	//}