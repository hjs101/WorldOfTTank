// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyBasePawn.h"
#include "SunnyEnemy.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyEnemy : public ASunnyBasePawn
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	class ASunnyTTank* TTank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRange = 300.f;
};
