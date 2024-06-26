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
void ATank::RotateTurret(float Value)
{
	TurretMesh->SetWorldRotation(
		FMath::RInterpTo(
			TurretMesh->GetComponentRotation(),
			FRotator(
				0,
				Value,//CameraComp->GetComponentRotation().Yaw,
				0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			0.5)
			);
}

float	LimitBarrelPitch(float Value)
{
	if (30 < Value)
		return 30;
	if (-10 > Value)
		return -10;
	return Value;
}

void	ATank::RotateBarrel(float Value)
{
	BarrelMesh->SetWorldRotation(
		FMath::RInterpTo(
			BarrelMesh->GetComponentRotation(),
			FRotator(
				LimitBarrelPitch(Value),//CameraComp->GetComponentRotation().Pitch,
				TurretMesh->GetComponentRotation().Yaw,
				0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			0.5)
			);
}