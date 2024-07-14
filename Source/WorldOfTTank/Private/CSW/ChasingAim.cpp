// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ChasingAim.h"

#include "Kismet/GameplayStatics.h"

void UChasingAim::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float fov = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetFOVAngle();
	if (fov <= 80)
	{
		Scale = 80 / fov;
		OuterCircle->SetRenderScale(FVector2D(Scale, Scale));
		OuterCircleBg->SetRenderScale(FVector2D(Scale * -1, Scale));
	}
}

void UChasingAim::SetIsHit(bool Value)
{
	IsHit = Value;
}

