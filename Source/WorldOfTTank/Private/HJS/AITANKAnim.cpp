// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/AITANKAnim.h"


void UAITANKAnim::SetWheelSpeedOffset(float Value)
{
	WheelSpeedOffset = Value;
}

float UAITANKAnim::GetWheelSpeedOffset()
{
	return WheelSpeedOffset;
}

void UAITANKAnim::SetWheelSpeed(float Value)
{
	WheelSpeed = Value;
}

float UAITANKAnim::GetWheelSpeed()
{
	return WheelSpeed;
}

void UAITANKAnim::SetWheelRotation(FRotator Value)
{
	WheelRotation = Value;
}

FRotator UAITANKAnim::GetWheelRotation()
{
	return WheelRotation;
}

void UAITANKAnim::UpdateTracksMat()
{
	if (TracksMID != nullptr)
	{
		float Val = WheelSpeedOffset*WheelSpeed*0.008333;
		TracksMID->SetScalarParameterValue("OffsetV", Val);
	}
	else
	{
		USkeletalMeshComponent* MySkel = GetOwningComponent();
		if (MySkel != nullptr)
		{
			UMaterialInstanceDynamic* Mat = Cast<UMaterialInstanceDynamic>(MySkel->GetMaterial(1));
			if (Mat != nullptr)
			{
				TracksMID = Mat;
			}
		}
	}
}

void UAITANKAnim::UpdateTurret()
{
	SetTurretRotation(FRotator(0.f, TurretAngle, 0.f));
	SetGunRotation(FRotator(GunElevation, TurretAngle,0.f));
}

void UAITANKAnim::SetTurretRotation(FRotator Value)
{
	TurretRotation = Value;
}

FRotator UAITANKAnim::GetTurretRotation()
{
	return TurretRotation;
}

void UAITANKAnim::SetGunRotation(FRotator Value)
{
	GunRotation = Value;
}

FRotator UAITANKAnim::GetGunRotation()
{
	return GunRotation;
}

void UAITANKAnim::SetTurretAngle(float Value)
{
	TurretAngle = Value;
}

float UAITANKAnim::GetTurretAngle()
{
	return TurretAngle;
}

void UAITANKAnim::SetGunElevation(float Value)
{
	GunElevation = Value;
}

float UAITANKAnim::GetGunElevation()
{
	return GunElevation;
}
