// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleAnimationInstance.h"
#include "AITANKAnim.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UAITANKAnim : public UVehicleAnimationInstance
{
	GENERATED_BODY()
	
public:
	void SetWheelSpeedOffset(float Value);
	float GetWheelSpeedOffset();
	void SetWheelSpeed(float Value);
	float GetWheelSpeed();
	void SetWheelRotation(FRotator Value);
	FRotator GetWheelRotation();
	
	UFUNCTION(BlueprintCallable)
	void UpdateTracksMat();

	UFUNCTION(BlueprintCallable)
	void UpdateTurret();

	void SetTurretRotation(FRotator Value);
	FRotator GetTurretRotation();
	void SetGunRotation(FRotator Value);
	FRotator GetGunRotation();
	void SetTurretAngle(float Value);
	float GetTurretAngle();
	void SetGunElevation(float Value);
	float GetGunElevation();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	float WheelSpeedOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	float WheelSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	FRotator WheelRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	UMaterialInstanceDynamic* TracksMID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	float TurretAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	float GunElevation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	FRotator TurretRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (allowprivateaccess = true));
	FRotator GunRotation;
};
