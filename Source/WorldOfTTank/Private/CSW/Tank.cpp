// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Tank.h"

#include "CSW/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	RootComponent = BaseMesh;
	BaseMesh->SetSimulatePhysics(true);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel Mesh"));
	BarrelMesh->SetupAttachment(TurretMesh);

	LeftWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWheel Mesh"));
	LeftWheelMesh->SetupAttachment(BaseMesh);
	LeftWheelMesh->SetSimulatePhysics(true);

	RightWheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWheel Mesh"));
	RightWheelMesh->SetupAttachment(BaseMesh);
	RightWheelMesh->SetSimulatePhysics(true);

	RightWheelSuspension = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RightWheel Suspension"));
	RightWheelSuspension->SetupAttachment(RootComponent);
	RightWheelSuspension->ComponentName1.ComponentName = "Base Mesh";
	RightWheelSuspension->ComponentName2.ComponentName = "RightWheel Mesh";
	
	LeftWheelSuspension = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftWheel Suspension"));
	LeftWheelSuspension->SetupAttachment(RootComponent);
	LeftWheelSuspension->ComponentName1.ComponentName = "Base Mesh";
	LeftWheelSuspension->ComponentName2.ComponentName = "LeftWheel Mesh";

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(BarrelMesh);
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	SetPlayerTankDamage(80);
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentReloadTime < ReloadTime)
	{
		if (CurrentReloadTime + DeltaTime > ReloadTime)
			CurrentReloadTime = ReloadTime;
		else
			CurrentReloadTime += DeltaTime;
	}
		
}

FVector ATank::GetCurrentHitPoint() const
{
	FVector Start = BarrelMesh->GetComponentLocation() ;
	FVector End = Start + BarrelMesh->GetForwardVector() * 100000000000;
	FHitResult	Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_WorldStatic);
	return Hit.Location;
}


void ATank::Move(float Value)
{
	MoveState = (Value >= 0);

	if (Speed < 500)
		Speed += 1;
	LeftWheelMesh->SetWorldLocation(LeftWheelMesh->GetComponentLocation() + Value * LeftWheelMesh->GetForwardVector() * Speed * UGameplayStatics::GetWorldDeltaSeconds(this));
	RightWheelMesh->SetWorldLocation(RightWheelMesh->GetComponentLocation() + Value * RightWheelMesh->GetForwardVector() * Speed * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void ATank::Turn(float Value)
{
	FRotator DeltaRotation = FRotator::ZeroRotator;
	
	if (!MoveState)
		Value *= -1;
	DeltaRotation.Yaw = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * TurnRate;
	LeftWheelMesh->AddLocalRotation(DeltaRotation, true);
	RightWheelMesh->AddLocalRotation(DeltaRotation, true);
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
	if (CurrentReloadTime != ReloadTime)
		return ;
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		ProjectileSpawnPoint->GetComponentLocation(),
		ProjectileSpawnPoint->GetComponentRotation());
	CurrentReloadTime = 0;

	Projectile->SetOwner(this);
}

void ATank::Brake()
{
	Speed = 150;
}

void ATank::SetPlayerTankDamage(float Damage)
{
	if (HP <= Damage)
		HP = 0;
	else
		HP -= Damage;	

}

