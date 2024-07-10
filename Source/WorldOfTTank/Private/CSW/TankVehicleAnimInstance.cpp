// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/TankVehicleAnimInstance.h"
#include <cmath>

void UTankVehicleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	RotateWheel(DeltaSeconds);
	if (!IsValid(Track))
		Track = Cast<UMaterialInstanceDynamic>(GetOwningComponent()->GetMaterial(1));
	else
		Track->SetScalarParameterValue(FName("OffsetV"), SpeedOffset * WheelSpeed * -0.008333);
}


void UTankVehicleAnimInstance::RotateWheel(float DeltaSeconds)
{
	SpeedOffset = std::fmod((SpeedOffset + DeltaSeconds),360.f);
	FRotator NewRotator = FRotator(0, SpeedOffset * WheelSpeed * -1, 0);
	WheelRotation = NewRotator;
}

float UTankVehicleAnimInstance::GetSpeedOffset() const
{
	return SpeedOffset;
}

float UTankVehicleAnimInstance::GetWheelSpeed() const
{
	return WheelSpeed;
}

float UTankVehicleAnimInstance::GetWheelTurnAngle() const
{
	return WheelTurnAngle;
}

float UTankVehicleAnimInstance::GetTurretRotation() const
{
	return TurretRotation;
}

float UTankVehicleAnimInstance::GetTurretElevation() const
{
	return TurretElevation;
}

FRotator UTankVehicleAnimInstance::GetWheelRotation() const
{
	return WheelRotation;
}

void UTankVehicleAnimInstance::SetSpeedOffset(float Value)
{
	SpeedOffset = Value;
}

void UTankVehicleAnimInstance::SetWheelSpeed(float Value)
{
	WheelSpeed = Value;
}

void UTankVehicleAnimInstance::SetWheelTurnAngle(float Value)
{
	WheelTurnAngle = Value;
}

void UTankVehicleAnimInstance::SetTurretRotation(float Value)
{
	TurretRotation = Value;
}

void UTankVehicleAnimInstance::SetTurretElevation(float Value)
{
	TurretElevation = Value;
}

void UTankVehicleAnimInstance::SetWheelRotation(const FRotator& Value)
{
	WheelRotation = Value;
}

void UTankVehicleAnimInstance::SetTrack(UMaterialInstanceDynamic* Value)
{
	Track = Value;
}

