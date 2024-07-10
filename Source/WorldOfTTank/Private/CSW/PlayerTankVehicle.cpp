// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/PlayerTankVehicle.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APlayerTankVehicle::APlayerTankVehicle()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 1000;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->FieldOfView = 80;
}

void APlayerTankVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void APlayerTankVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerTankVehicle::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerTankVehicle::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &APlayerTankVehicle::LookRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APlayerTankVehicle::LookUpDown);

	// PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &APlayerTankVehicle::ZoomIn);
	// PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &APlayerTankVehicle::ZoomOut);
	//
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APlayerTankVehicle::Fire);
	PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &APlayerTankVehicle::Brake);
}


void APlayerTankVehicle::LookRightLeft(float Value) 
{
	AddControllerYawInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	RotateTurret(CameraComp->GetComponentRotation().Yaw - GetRootComponent()->GetComponentRotation().Yaw);
}

FVector APlayerTankVehicle::GetCursorTarget() const
{
	FVector Start = CameraComp->GetComponentLocation();
	FVector End = CameraComp->GetComponentLocation() + CameraComp->GetComponentRotation().Vector() * 15000000;
	FHitResult	Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start , End, ECollisionChannel::ECC_WorldStatic))
		return Hit.Location;
	return FVector::ZeroVector;
}

void APlayerTankVehicle::LookUpDown(float Value)
{
	AddControllerPitchInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	FVector Target = GetCursorTarget();
	RotateBarrel(Target);
}

