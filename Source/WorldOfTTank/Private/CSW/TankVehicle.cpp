// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/TankVehicle.h"
#include "ChaosVehicleMovementComponent.h"
#include "CSW/Projectile.h"
#include "CSW/TankVehicleAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ATankVehicle::ATankVehicle()
{
	UChaosVehicleMovementComponent* MoveComp = GetVehicleMovementComponent();
	MoveComp->SetIsReplicated(true);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent, FName("gunSocket"));

	GunFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GunFire"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/CSW/VigilanteContent/Vehicles/East_SPG_2S3Akatsia/FX/FX_MuzzleFire_01_2S3Akatsia.FX_MuzzleFire_01_2S3Akatsia'"));
	if (ParticleAsset.Succeeded())
	{
		GunFire->SetTemplate(ParticleAsset.Object);
	}
	GunFire->SetupAttachment(ProjectileSpawnPoint);
}

void ATankVehicle::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->CreateDynamicMaterialInstance(1);
	GetMesh()->SetAnimInstanceClass(AnimInstanceClass);
}

void ATankVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (CurrentReloadTime + DeltaSeconds < ReloadTime)
		CurrentReloadTime += DeltaSeconds;
	else if (CurrentReloadTime + DeltaSeconds > DeltaSeconds)
		CurrentReloadTime = ReloadTime;
	
	UTankVehicleAnimInstance* animIns = Cast<UTankVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	float speed = GetVehicleMovementComponent()->GetForwardSpeed();
	
	animIns->SetWheelSpeed(FMath::Clamp(speed, -500, 500));
}

void ATankVehicle::Move(float Value)
{
	UChaosVehicleMovementComponent* moveComp = GetVehicleMovementComponent();

	MoveState = Value;
	if (Value > 0)
	{
		moveComp->SetThrottleInput(Value);
		moveComp->SetBrakeInput(0);
	}
	else if (Value < 0)
	{
		moveComp->SetThrottleInput(Value);
		moveComp->SetBrakeInput(-1 * Value);
	}
}

void ATankVehicle::Turn(float Value)
{
	UChaosVehicleMovementComponent* moveComp = GetVehicleMovementComponent();

	if (MoveState == 0)
		moveComp->SetThrottleInput(0.4f);
	else if (MoveState < 0)
		Value *= -1;
	if (GetMesh()->GetPhysicsAngularVelocityInDegrees().Length() < 30)
		moveComp->SetYawInput(Value/4);
	else
		moveComp->SetYawInput(0);
}

void ATankVehicle::RotateTurret(float Value)
{
	UTankVehicleAnimInstance* animIns = Cast<UTankVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->SetTurretRotation(
		FMath::FInterpConstantTo(
			animIns->GetTurretRotation(),
			Value,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35)
			);
}

void	ATankVehicle::RotateBarrel(FVector Target)
{
	UTankVehicleAnimInstance* animIns = Cast<UTankVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->SetTurretElevation(
		FMath::FInterpConstantTo(
			animIns->GetTurretElevation(),
			FMath::Clamp(Target.Rotation().Pitch - animIns->GetTurretElevation(), -5, 60),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35)
			);
}

void ATankVehicle::Fire()
{
	if (CurrentReloadTime != ReloadTime)
		return ;
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileClass,
		ProjectileSpawnPoint->GetComponentLocation(),
		ProjectileSpawnPoint->GetComponentRotation());
	CurrentReloadTime = 0;
	GunFire->Activate();

	
	Projectile->SetOwner(this);
}

void ATankVehicle::Brake()
{
	MoveState = 0;
	GetVehicleMovementComponent()->SetThrottleInput(0);
	GetVehicleMovementComponent()->SetBrakeInput(1);
}