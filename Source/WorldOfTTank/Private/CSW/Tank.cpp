// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Tank.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	RootComponent = BaseMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel Mesh"));
	BarrelMesh->SetupAttachment(TurretMesh);

	LeftWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWhell Mesh"));
	LeftWheelMesh->SetupAttachment(BaseMesh);

	RightWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RigthWhell Mesh"));
	RightWheelMesh->SetupAttachment(BaseMesh);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 1000;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->FieldOfView = 80;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTurretAndBarrel();
}

void	ATank::LookRightLeft(float value)
{
	AddControllerYawInput(value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
}

void ATank::LookUpDown(float value)
{
	if ((CameraComp->GetComponentRotation().Pitch >= 30 && value < 0) ||
		(CameraComp->GetComponentRotation().Pitch <= -50) && value > 0)
		return ;
	AddControllerPitchInput(value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
}


// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &ATank::LookRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &ATank::LookUpDown);

	PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &ATank::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &ATank::ZoomOut);
}


void	ATank::Move(float Value)
{
	FVector	DeltaLocation = FVector::ZeroVector;

	MoveState = (Value >= 0);
	DeltaLocation.X = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * Speed;
	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(float Value)
{
	FRotator DeltaRotation = FRotator::ZeroRotator;

	if (!MoveState)
		Value *= -1;
	DeltaRotation.Yaw = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * TurnRate;
	AddActorLocalRotation(DeltaRotation, true);
}

void ATank::ZoomIn()
{
	if (SpringArmComp->TargetArmLength > -200)
		SpringArmComp->TargetArmLength -= 300;
	else if (SpringArmComp->TargetArmLength == -200)
	{
		if (CameraComp->FieldOfView > 5)
			CameraComp->FieldOfView /= 2;
	}
}

void ATank::ZoomOut()
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


float	LimitRotationAngle(float angle)
{
	if (angle > 80)
		return 80;
	if (angle < -20)
		return -20;
	return angle;
}

void ATank::RotateTurretAndBarrel()
{
	TurretMesh->SetWorldRotation(
		FMath::RInterpTo(
			TurretMesh->GetComponentRotation(),
			FRotator(
				0,
				CameraComp->GetComponentRotation().Yaw,
				0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			0.5)
			);

	BarrelMesh->SetWorldRotation(
		FMath::RInterpTo(
			BarrelMesh->GetComponentRotation(),
			FRotator(
				LimitRotationAngle(CameraComp->GetComponentRotation().Pitch),
				TurretMesh->GetComponentRotation().Yaw,
				0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			0.5)
			);
}
