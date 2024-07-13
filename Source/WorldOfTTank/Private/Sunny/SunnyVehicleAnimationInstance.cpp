// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyVehicleAnimationInstance.h"
#include <cmath>

void USunnyVehicleAnimationInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	RotateWheel(DeltaSeconds);
	if (!IsValid(Track))
		Track = Cast<UMaterialInstanceDynamic>(GetOwningComponent()->GetMaterial(1));
	else
		Track->SetScalarParameterValue(FName("OffsetV"), SpeedOffset * WheelSpeed * -0.008333);
}


void USunnyVehicleAnimationInstance::RotateWheel(float DeltaSeconds)
{
	SpeedOffset = std::fmod((SpeedOffset + DeltaSeconds), 360.f);
	FRotator NewRotator = FRotator(0, SpeedOffset * WheelSpeed * -1, 0);
	WheelRotation = NewRotator;
}

float USunnyVehicleAnimationInstance::GetSpeedOffset() const
{
	return SpeedOffset;
}

float USunnyVehicleAnimationInstance::GetWheelSpeed() const
{
	return WheelSpeed;
}

float USunnyVehicleAnimationInstance::GetWheelTurnAngle() const
{
	return WheelTurnAngle;
}

FRotator USunnyVehicleAnimationInstance::GetTurretRotation() const
{
	return TurretRotation;
}

FRotator USunnyVehicleAnimationInstance::GetGunElevation() const
{
	return GunElevation;
}

FRotator USunnyVehicleAnimationInstance::GetWheelRotation() const
{
	return WheelRotation;
}

void USunnyVehicleAnimationInstance::SetSpeedOffset(float Value)
{
	SpeedOffset = Value;
}

void USunnyVehicleAnimationInstance::SetWheelSpeed(float Value)
{
	WheelSpeed = Value;
}

void USunnyVehicleAnimationInstance::SetWheelTurnAngle(float Value)
{
	WheelTurnAngle = Value;
}

void USunnyVehicleAnimationInstance::SetTurretRotation(const FRotator& Value)
{
	TurretRotation = Value;
}

void USunnyVehicleAnimationInstance::SetGunElevation(const FRotator& Value)
{
	GunElevation = Value;
}

void USunnyVehicleAnimationInstance::SetWheelRotation(const FRotator& Value)
{
	WheelRotation = Value;
}

void USunnyVehicleAnimationInstance::SetTrack(UMaterialInstanceDynamic* Value)
{
	Track = Value;
}

