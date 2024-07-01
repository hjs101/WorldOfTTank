// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/PlayerTank.h"

#include "CollisionDebugDrawingPublic.h"
#include "FrameTypes.h"
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

void APlayerTank::BeginPlay()
{
	Super::BeginPlay();
	
}


void APlayerTank::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector End = CameraComp->GetComponentLocation() + CameraComp->GetComponentRotation().Vector() * 15000000;
	FHitResult	Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraComp->GetComponentLocation(), End, ECollisionChannel::ECC_GameTraceChannel1))
		DrawDebugPoint(GetWorld(), Hit.Location, 1, FColor::Red, false, -1);
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

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APlayerTank::Fire);
	PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &APlayerTank::Brake);
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
	FVector End = CameraComp->GetComponentLocation() + CameraComp->GetComponentRotation().Vector() * 15000000;
	FHitResult	Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, CameraComp->GetComponentLocation(), End, ECollisionChannel::ECC_GameTraceChannel1))
		RotateBarrel(FVector(Hit.Location.X, Hit.Location.Y, Hit.Location.Z));
}

void APlayerTank::ZoomWithArmLength()
{
	// float pre = SpringArmComp->TargetArmLength;
	// SpringArmComp->TargetArmLength = FMath::FInterpTo(
	// 	SpringArmComp->TargetArmLength,
	// 	EndCamValue,
	// 	GetWorld()->GetDeltaSeconds(),
	// 	10);
	// UE_LOG(LogTemp, Warning, TEXT("%f"), SpringArmComp->TargetArmLength);
	// if (pre == SpringArmComp->TargetArmLength)
	// {
	// 	SpringArmComp->TargetArmLength = EndCamValue;
	// 	return ;
	// }
	// FTimerHandle TimerHandle;
	// GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerTank::ZoomWithArmLength, 0.01f, false);
}


int	APlayerTank::GetZoomIndex(float Arm)
{
	int i = 0;
	while (Arm != ArmLength[i])
		i++;
	return i == 6 ? -1 : i;
}

void APlayerTank::ZoomIn()
{
	int idx = GetZoomIndex(SpringArmComp->TargetArmLength);
	if (idx < 0)
		return ;
	if (SpringArmComp->TargetArmLength > -200)
	{
		ArmLengthIdx = idx;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerTank::ZoomWithArmLength, 0.1f, false);
	}
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