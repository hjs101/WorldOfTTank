// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/TankWheel.h"

UTankWheel::UTankWheel()
{
	WheelRadius = 32;
	WheelWidth = 40;
	WheelMass = 500;
	SkidThreshold = 1000;
	SlipThreshold = 1000;
	bAffectedByBrake = true;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
	bABSEnabled = true;
	SuspensionMaxRaise = 10;
	SuspensionMaxDrop = 10;
	SpringRate = 250;
	SuspensionSmoothing = 1;
}

