// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "HJS/AITank_1.h"
#include "HJS/AIProjecttile_1.h"
// Sets default values
AAITank_1::AAITank_1()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Box Comp"));
	SetRootComponent(CapsuleComp);
	// 스태틱 메시 컴포넌트(탱크) 생성
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Body"));
	BodyMesh->SetupAttachment(RootComponent);
	WheelMesh_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Wheel1"));
	WheelMesh_1->SetupAttachment(RootComponent);
	WheelMesh_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Wheel2"));
	WheelMesh_2->SetupAttachment(RootComponent);
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Head"));
	HeadMesh->SetupAttachment(RootComponent);
	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Barrel"));
	BarrelMesh->SetupAttachment(HeadMesh);

	// 스폰 포인트 생성
	ProjecttileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AITank_1 Spawn Point"));
	ProjecttileSpawnPoint->SetupAttachment(BarrelMesh);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("AITank_1 MovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	// Tank physics settings
	if (CapsuleComp){
		CapsuleComp->SetSimulatePhysics(true);
		CapsuleComp->SetLinearDamping(1.0f); // 선형 감쇠
		CapsuleComp->SetAngularDamping(1.0f); // 각 감쇠
	}
}

// Called when the game starts or when spawned
void AAITank_1::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAITank_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAITank_1::Move(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("%f"),value);
	//FVector DeltaLocation = FVector::ZeroVector;
	//// X = Value * DeltaTime * Speed
	//DeltaLocation.X = value * UGameplayStatics::GetWorldDeltaSeconds(this) * Speed;
	//AddActorLocalOffset(DeltaLocation, true);
	MoveState = value;
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorForwardVector() * value);
	}

}

void AAITank_1::BodyTurn(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("BodyTurn!!"));
	FRotator DeltaRotation = FRotator::ZeroRotator;
	
	if (MoveState== -1) {
		value = -1*value;
	}
	
	DeltaRotation.Yaw = value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	AddActorLocalRotation(DeltaRotation, true);
	if (MoveState == 0 && MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorForwardVector()/2 * abs(value));
	}
}

void AAITank_1::Fire()
{
	FVector ProjecttileSpawnPointLocation = ProjecttileSpawnPoint->GetComponentLocation();
	FRotator ProjecttileSpawnPoinRotation = ProjecttileSpawnPoint->GetComponentRotation();
	if(ProjecttileClass){
		AAIProjecttile_1* Projectile = GetWorld()->SpawnActor<AAIProjecttile_1>(ProjecttileClass, ProjecttileSpawnPointLocation, ProjecttileSpawnPoinRotation);
		Projectile->SetOwner(this);
	}
}

void AAITank_1::RotateTurret(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - HeadMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	// 머리 돌리기
	HeadMesh->SetWorldRotation(
		FMath::RInterpTo(HeadMesh->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
			, 2.f)
	);

	// 포신 각도 설정
	LookAtRotation.Pitch = ToTarget.Rotation().Pitch;
	// 포신 각도 제한
	if (LookAtRotation.Pitch < DownLimit) {
		LookAtRotation.Pitch = DownLimit;
	}
	if (LookAtRotation.Pitch > UpLimit) {
		LookAtRotation.Pitch = UpLimit;
	}
	// 포신 돌리기
	BarrelMesh->SetWorldRotation(
		FMath::RInterpTo(BarrelMesh->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
		, 3.f)
	);

}

void AAITank_1::RotateTank(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - CapsuleComp->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	// 몸체 돌리기
	CapsuleComp->SetWorldRotation(
		FMath::RInterpTo(CapsuleComp->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
			, 5.f)
	);

}