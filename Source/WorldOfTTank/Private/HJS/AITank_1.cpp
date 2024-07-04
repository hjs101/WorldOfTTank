// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "HJS/AITank_1.h"
#include "HJS/AIProjecttile_1.h"
// Sets default values
AAITank_1::AAITank_1()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Comp"));
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
	MoveState = value;
	//if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	//{
	//	MovementComponent->AddInputVector(GetActorForwardVector() * value);
	//}
	CapsuleComp->AddForce((GetActorForwardVector()*1000000.f*value));
}

void AAITank_1::BodyTurn(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("BodyTurn!!"));
	FRotator DeltaRotation = FRotator::ZeroRotator;
	if (MoveState == -1) {
		value = -1 * value;
	}

	DeltaRotation.Yaw = value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	AddActorLocalRotation(DeltaRotation, true);
	if (MoveState == 0 && MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorForwardVector() / 2 * abs(value));
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
		FMath::RInterpConstantTo(HeadMesh->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
			, 120.f)
	);

	//LookAtRotation.Pitch = ToTarget.Rotation().Pitch;
	// 포신 각도 설정 : 탄도학 적용 버전
	float TargetDistance = ToTarget.Size2D()/100.f; // 수평 거리
	float TargetHeight = ToTarget.Z/100.f; // 높이 차이
	LookAtRotation.Pitch = CalculateLaunchAngle(FireSpeed, TargetDistance, TargetHeight);
	LookPitch = LookAtRotation.Pitch;
	// 포신 각도 제한
	if (LookAtRotation.Pitch < DownLimit) {
		LookAtRotation.Pitch = DownLimit;
	}
	if (LookAtRotation.Pitch > UpLimit) {
		LookAtRotation.Pitch = UpLimit;
	}
	// 포신 돌리기
	BarrelMesh->SetWorldRotation(
		FMath::RInterpConstantTo(BarrelMesh->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
		, 80.f)
	);

}

void AAITank_1::RotateTank(FVector LookAtTarget)
{

	if (LookAtTarget == FVector::ZeroVector) {
		return;
	}
	FVector ToTarget = LookAtTarget - CapsuleComp->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	// 몸체 돌리기
	CapsuleComp->SetWorldRotation(
		FMath::RInterpTo(CapsuleComp->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
			, 2.f)
	);

}

float AAITank_1::CalculateLaunchAngle(float LaunchSpeed, float TargetDistance, float TargetHeight)
{
	const float Gravity = 9.8f;
	float LaunchSpeedSquared = LaunchSpeed * LaunchSpeed;
	float Term1 = LaunchSpeedSquared * LaunchSpeedSquared - Gravity * (Gravity * TargetDistance * TargetDistance + 2 * TargetHeight * LaunchSpeedSquared);

	if (Term1 < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid solution exists for the given parameters."));
		return 0.0f;
	}

	Term1 = sqrt(Term1);
	float Term2 = Gravity * TargetDistance;

	float Angle1 = atan((LaunchSpeedSquared + Term1) / Term2);
	float Angle2 = atan((LaunchSpeedSquared - Term1) / Term2);

	// 선택: 두 개의 각도 중 하나를 선택 (낮은 각도 또는 높은 각도)
	return FMath::RadiansToDegrees(Angle2); // 또는 FMath::RadiansToDegrees(Angle2)
}