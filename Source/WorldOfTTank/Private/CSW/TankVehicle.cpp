// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/TankVehicle.h"
#include "ChaosVehicleMovementComponent.h"
#include "CSW/Projectile.h"
#include "CSW/TankVehicleAnimInstance.h"
#include "HJS/AITankCPU_1.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"

ATankVehicle::ATankVehicle()
{
	UChaosVehicleMovementComponent* MoveComp = GetVehicleMovementComponent();
	MoveComp->SetIsReplicated(true);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent, FName("gun_jnt"));
	ProjectileSpawnPoint->SetRelativeLocation(FVector(500, 0, 0));

	GunFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GunFire"));
	ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/CSW/VigilanteContent/Vehicles/East_SPG_2S3Akatsia/FX/FX_MuzzleFire_01_2S3Akatsia.FX_MuzzleFire_01_2S3Akatsia'"));
	if (ParticleAsset.Succeeded())
	{
		GunFire->SetTemplate(ParticleAsset.Object);
		GunFire->bAutoActivate = false;
	}
	GunFire->SetupAttachment(ProjectileSpawnPoint);

	FireSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSoundComp"));
	FireSoundComp->SetupAttachment(RootComponent);
	FireSoundComp->bAutoActivate = false;

	TrackSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TrackSoundComp"));
	TrackSoundComp->SetupAttachment(RootComponent);
	TrackSoundComp->bAutoActivate = false;
}

void ATankVehicle::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->CreateDynamicMaterialInstance(1);
	GetMesh()->SetAnimInstanceClass(AnimInstanceClass);

	FireSoundComp->SetSound(FireSound);
	TrackSoundComp->SetSound(TrackSound);
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

USceneComponent* ATankVehicle::GetProjectileSpawnPoint() const
{
	return ProjectileSpawnPoint;
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

	// if (MoveState == 0)
	// 	moveComp->SetThrottleInput(1);
	if (MoveState < 0)
		Value *= -1;

	if (GetMesh()->GetPhysicsAngularVelocityInDegrees().Length() < 30)
		moveComp->SetYawInput(Value);
	else
		moveComp->SetYawInput(Value/30);
}

void ATankVehicle::RotateTurret(float Value)
{
	UTankVehicleAnimInstance* animIns = Cast<UTankVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	FRotator start = animIns->GetTurretRotation();
	FRotator end = FRotator(0, Value, 0);
	FRotator lerp = FMath::RInterpConstantTo(
			start,
			end,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35);
	animIns->SetTurretRotation(lerp);
}

void	ATankVehicle::RotateBarrel(float Value)
{
	UTankVehicleAnimInstance* animIns = Cast<UTankVehicleAnimInstance>(GetMesh()->GetAnimInstance());
	FRotator start = animIns->GetTurretElevation();
	FRotator end = FRotator(Value, animIns->GetTurretRotation().Yaw, 0);
	FRotator lerp = FMath::RInterpConstantTo(
			start,
			end,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35);
	animIns->SetTurretElevation(lerp);
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
	FireSoundComp->Play(0.f);
	
	Projectile->SetOwner(this);
}

void ATankVehicle::Brake()
{
	MoveState = 0;
	GetVehicleMovementComponent()->SetThrottleInput(0);
	GetVehicleMovementComponent()->SetBrakeInput(0);
}

void ATankVehicle::GetCurrentHitPoint(FHitResult& Hit) const
{
	FVector Start = ProjectileSpawnPoint->GetComponentLocation() ;
	FVector End = Start + ProjectileSpawnPoint->GetForwardVector() * 100000000000;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_WorldStatic, Params);
}

void ATankVehicle::SetPlayerTankDamage(float Damage)
{
	if (Hp > Damage)
		Hp -= Damage;
	else
		Hp = 0;
}

void ATankVehicle::UpdateMovementSound()
{
	FVector Velocity = GetMesh()->GetPhysicsLinearVelocity();
	float Speed = Velocity.Size();

	if (Speed > 0.0f){
		if (!TrackSoundComp->IsPlaying())
			TrackSoundComp->Play();

		float Pitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f,1000.f), FVector2D(0.5f,1.5f),Speed);
		TrackSoundComp->SetPitchMultiplier(Pitch);
	}
	else
	{
		if (TrackSoundComp->IsPlaying())
			TrackSoundComp->Stop();
	}
}