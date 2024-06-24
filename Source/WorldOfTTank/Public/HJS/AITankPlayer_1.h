// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HJS/AITank_1.h"
#include "AITankPlayer_1.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API AAITankPlayer_1 : public AAITank_1
{
	GENERATED_BODY()

public:
	AAITankPlayer_1();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool bAlive = true;
protected:


private:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* CameraComp;

};
