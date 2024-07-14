// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyRealTankPawn.h"
#include "TestSunnyNewPawn.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ATestSunnyNewPawn : public ASunnyRealTankPawn
{
	GENERATED_BODY()
public:
	ATestSunnyNewPawn();
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* CameraComp;
};
