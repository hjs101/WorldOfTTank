// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Tank.h"

#include "CollisionDebugDrawingPublic.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CSW/Projectile.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	RootComponent = BaseMesh;
	BaseMesh->SetSimulatePhysics(true);
	BaseMesh->SetLinearDamping(0.8f);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel Mesh"));
	BarrelMesh->SetupAttachment(TurretMesh);

	LeftWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWhell Mesh"));
	LeftWheelMesh->SetupAttachment(BaseMesh);

	RightWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RigthWhell Mesh"));
	RightWheelMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(BarrelMesh);

	MoveComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Move Comp"));
	MoveComp->SetUpdatedComponent(RootComponent);
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
	MoveState = (Value >= 0);
	MoveComp->AddInputVector(GetActorForwardVector() * Value);
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
		FMath::RInterpConstantTo(
			TurretMesh->GetComponentRotation(),
			FRotator(
				0,
				Value,
				0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35)
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

void	ATank::RotateBarrel(FVector Target)
{
	BarrelMesh->SetWorldRotation(
		FMath::RInterpConstantTo(
			BarrelMesh->GetComponentRotation(),
			FRotator(
				LimitBarrelPitch((Target - BarrelMesh->GetComponentLocation()).Rotation().Pitch),
				TurretMesh->GetComponentRotation().Yaw,
				0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35)
			);
}

void ATank::Fire()
{
	GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		ProjectileSpawnPoint->GetComponentLocation(),
		ProjectileSpawnPoint->GetComponentRotation());
}

void ATank::Brake()
{
	MoveComp->Velocity /= 2;
}

