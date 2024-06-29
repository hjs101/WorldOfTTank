// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyTTank.h"
#include "Kismet/GameplayStatics.h"

void ASunnyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Find the distance to the Tank
	if (TTank)
	{
		float Distance = FVector::Dist(GetActorLocation(), TTank->GetActorLocation());

		// Check to see if the Tank is in range
		if (Distance <= FireRange)
		{
			// If in range, rotate turret towrad Tank
			RotateTurret(TTank->GetActorLocation());
		}
	}
}

void ASunnyEnemy::BeginPlay()
{
	Super::BeginPlay();

	TTank = Cast<ASunnyTTank>(UGameplayStatics::GetPlayerPawn(this, 0));
}