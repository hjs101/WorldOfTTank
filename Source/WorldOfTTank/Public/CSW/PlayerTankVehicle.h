// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/TankVehicle.h"
#include "PlayerTankVehicle.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API APlayerTankVehicle : public ATankVehicle
{
	GENERATED_BODY()

public:
	APlayerTankVehicle();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	FVector GetCursorTarget() const;

	
protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UCameraComponent* CameraComp;
	
private:
	float ViewRotationRate = 100;
	void	LookRightLeft(float Value);
	void	LookUpDown(float Value);
};
