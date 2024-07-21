// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/TankVehicle.h"
#include "ChaosVehicleMovementComponent.h"
#include "NiagaraComponent.h"
#include "CSW/Projectile.h"
#include "CSW/TankVehicleAnimInstance.h"
#include "HJS/AITankCPU_1.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "HJS/FractureWall.h"
ATankVehicle::ATankVehicle()
{
	UChaosVehicleMovementComponent* MoveComp = GetVehicleMovementComponent();
	MoveComp->SetIsReplicated(true);

	DieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DieMesh"));
	DieMesh->SetupAttachment(RootComponent);
	
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

	DieFire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DieFire"));
	DieFire->SetupAttachment(RootComponent);
	DieFire->bAutoActivate = false;

	GunRipple = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GunRipple"));
	GunRipple->bAutoActivate = false;
	GunRipple->SetupAttachment(ProjectileSpawnPoint);

	InnerFireSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("InnerFireSoundComp"));
	InnerFireSoundComp->SetupAttachment(RootComponent);
	InnerFireSoundComp->bAutoActivate = false;

	OuterFireSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("OuterFireSoundComp"));
	OuterFireSoundComp->SetupAttachment(RootComponent);
	OuterFireSoundComp->bAutoActivate = false;

	HitSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSoundComp"));
	HitSoundComp->SetupAttachment(RootComponent);
	HitSoundComp->bAutoActivate = false;

	TrackSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TrackSoundComp"));
	TrackSoundComp->SetupAttachment(RootComponent);
	TrackSoundComp->bAutoActivate = false;
}

void ATankVehicle::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->CreateDynamicMaterialInstance(1);
	GetMesh()->SetAnimInstanceClass(AnimInstanceClass);

	GetMesh()->SetCollisionProfileName(TEXT("BlockAll"));
	GetMesh()->SetNotifyRigidBodyCollision(true);  // Hit 이벤트 활성화

	GetMesh()->OnComponentHit.AddDynamic(this, &ATankVehicle::OnHit);
	DieMesh->SetVisibility(false);
	DieMesh->SetCollisionProfileName("NoCollision");
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
	UpdateMovementSound();
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
	GunRipple->Activate();
	InnerFireSoundComp->Play(0.3f);
	OuterFireSoundComp->Play(0.f);
	GetMesh()->AddImpulse(-2000000 * ProjectileSpawnPoint->GetForwardVector());
	
	Projectile->SetOwner(this);
}

void ATankVehicle::Brake()
{
	MoveState = 0;
	GetVehicleMovementComponent()->SetThrottleInput(0);
	GetVehicleMovementComponent()->SetBrakeInput(0);
}

void ATankVehicle::Die()
{
	GetMesh()->SetVisibility(false);
	GetMesh()->SetCollisionProfileName("NoCollision");
	DieMesh->SetVisibility(true);
	DieMesh->SetCollisionProfileName("Vehicle");

	DieFire->Activate();
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
	{
		Hp = 0;
		Die();
	}
}

void ATankVehicle::UpdateMovementSound()
{
	FVector Velocity = GetMesh()->GetPhysicsLinearVelocity();
	float Speed = Velocity.Size();
	
		if (!TrackSoundComp->IsPlaying())
			TrackSoundComp->Play();

	float Pitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f,1000.f), FVector2D(0.5f,1.5f),Speed);
	TrackSoundComp->SetPitchMultiplier(Pitch);
}


UAudioComponent* ATankVehicle::GetOuterFireSoundComp() const
{
	return OuterFireSoundComp;
}

UAudioComponent* ATankVehicle::GetInnerFireSoundComp() const
{
	return InnerFireSoundComp;
}

UAudioComponent* ATankVehicle::GetTrackSoundComp() const
{
	return TrackSoundComp;
}

void ATankVehicle::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	AFractureWall* Wall = Cast<AFractureWall>(OtherActor);

	if (Wall != nullptr)
	{
		Wall->SetDestroyTimer();
		Wall->SetActorEnableCollision(false);
	}
	
	HitSoundComp->Activate();
}
