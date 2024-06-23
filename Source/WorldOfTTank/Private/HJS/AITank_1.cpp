// Fill out your copyright notice in the Description page of Project Settings.
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "HJS/AITank_1.h"
#include "HJS/AIProjecttile_1.h"
// Sets default values
AAITank_1::AAITank_1()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스태틱 메시 컴포넌트(탱크) 생성
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Body"));
	SetRootComponent(BodyMesh);
	WhellMesh_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Wheel1"));
	WhellMesh_1->SetupAttachment(RootComponent);
	WhellMesh_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Wheel2"));
	WhellMesh_2->SetupAttachment(RootComponent);
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Head"));
	HeadMesh->SetupAttachment(RootComponent);
	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AITank_1 Barrel"));
	BarrelMesh->SetupAttachment(RootComponent);

	// 스폰 포인트 생성
	ProjecttileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AITank_1 Spawn Point"));
	ProjecttileSpawnPoint->SetupAttachment(HeadMesh);

	// 스프링 암 생성
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("AITank_1 Spring Arm"));
	// 스프링 암을 루트 컴포넌트에 붙이기
	SpringArmComp->SetupAttachment(RootComponent);
	// 카메라 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("AITank_1 Camera"));
	// 카메라를 스프링 암에 붙이기
	CameraComp->SetupAttachment(SpringArmComp);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("AITank_1 MovementComponent"));
	MovementComponent->SetUpdatedComponent(BodyMesh);

	// Tank physics settings
	if (BodyMesh){
		BodyMesh->SetMassOverrideInKg(NAME_None, 2000.0f); // 탱크의 질량 증가
		BodyMesh->SetSimulatePhysics(true);
		BodyMesh->SetLinearDamping(1.0f); // 선형 감쇠
		BodyMesh->SetAngularDamping(1.0f); // 각 감쇠
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

// Called to bind functionality to input
void AAITank_1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AAITank_1::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AAITank_1::BodyTurn);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AAITank_1::Fire);
}
void AAITank_1::Move(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("%f"),value);
	//FVector DeltaLocation = FVector::ZeroVector;
	//// X = Value * DeltaTime * Speed
	//DeltaLocation.X = value * UGameplayStatics::GetWorldDeltaSeconds(this) * Speed;
	//AddActorLocalOffset(DeltaLocation, true);
	moveState = value;
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorForwardVector() * value);
	}

}

void AAITank_1::BodyTurn(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("BodyTurn!!"));
	FRotator DeltaRotation = FRotator::ZeroRotator;
	
	if (moveState== -1) {
		value = -1*value;
	}
	
	DeltaRotation.Yaw = value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	AddActorLocalRotation(DeltaRotation, true);
	if (moveState == 0 && MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
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
