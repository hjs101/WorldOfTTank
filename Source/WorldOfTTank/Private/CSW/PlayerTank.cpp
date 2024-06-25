// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/PlayerTank.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerTank::APlayerTank()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 1000;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->FieldOfView = 80;
}

// Called to bind functionality to input
void APlayerTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerTank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerTank::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &APlayerTank::LookRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APlayerTank::LookUpDown);

	PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &APlayerTank::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &APlayerTank::ZoomOut);
}

void	APlayerTank::LookRightLeft(float Value)
{
	AddControllerYawInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	RotateTurret(CameraComp->GetComponentRotation().Yaw);
}

void APlayerTank::LookUpDown(float Value)
{
	if ((CameraComp->GetComponentRotation().Pitch >= 30 && Value < 0) ||
		(CameraComp->GetComponentRotation().Pitch <= -50) && Value > 0)
		return ;
	AddControllerPitchInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	RotateBarrel(CameraComp->GetComponentRotation().Pitch);
}

void APlayerTank::ZoomIn()
{
	if (SpringArmComp->TargetArmLength > -200)
		SpringArmComp->TargetArmLength -= 300;
	else if (SpringArmComp->TargetArmLength == -200)
	{
		if (CameraComp->FieldOfView > 5)
			CameraComp->FieldOfView /= 2;
	}
}

void APlayerTank::ZoomOut()
{
	if (SpringArmComp->TargetArmLength == -200)
	{
		if (CameraComp->FieldOfView < 80)
			CameraComp->FieldOfView *= 2;
		else
			SpringArmComp->TargetArmLength += 300;
	}
	else if (SpringArmComp->TargetArmLength < 1300)
		SpringArmComp->TargetArmLength += 300;
}