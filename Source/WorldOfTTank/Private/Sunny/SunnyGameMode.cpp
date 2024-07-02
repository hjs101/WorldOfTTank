// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyGameMode.h"
#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyPlayerController.h"
#include "Kismet/GameplayStatics.h"


void ASunnyGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == TTank)
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
	}
}


void ASunnyGameMode::BeginPlay()
{
	Super::BeginPlay();

	TTank = Cast<ASunnyTTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	TTanksPlayerController = Cast<ASunnyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}