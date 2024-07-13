// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleAnimationInstance.h"
#include "SunnyVehicleAnimationInstance.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API USunnyVehicleAnimationInstance : public UVehicleAnimationInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void RotateWheel(float DeltaSeconds);
	
	UFUNCTION()
	float GetSpeedOffset() const;
	UFUNCTION()
	float GetWheelSpeed() const;
	UFUNCTION()
	float GetWheelTurnAngle() const;
	UFUNCTION()
	FRotator GetTurretRotation() const;
	UFUNCTION()
	FRotator GetGunElevation() const;
	UFUNCTION()
	FRotator GetWheelRotation() const;

	UFUNCTION()
	void SetSpeedOffset(float Value);
	UFUNCTION()
	void SetWheelSpeed(float Value);
	UFUNCTION()
	void SetWheelTurnAngle(float Value);
	UFUNCTION()
	void SetTurretRotation(const FRotator& Value);
	UFUNCTION()
	void SetGunElevation(const FRotator& Value);
	UFUNCTION()
	void SetWheelRotation(const FRotator& Value);
	UFUNCTION()
	void SetTrack(UMaterialInstanceDynamic* Value);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float SpeedOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float WheelSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float WheelTurnAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FRotator TurretRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FRotator GunElevation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FRotator WheelRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UMaterialInstanceDynamic* Track;
	
};
