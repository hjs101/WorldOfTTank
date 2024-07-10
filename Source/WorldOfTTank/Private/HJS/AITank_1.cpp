// Fill out your copyright notice in the Description page of Project Settings.
#include "HJS/AITank_1.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "HJS/AIProjecttile_1.h"
#include "Components/SkeletalMeshComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "HJS/AITANKAnim.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
// Sets default values
AAITank_1::AAITank_1()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(GetMesh());

	// 스폰 포인트 생성
	ProjecttileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AITank_1 Spawn Point"));
	ProjecttileSpawnPoint->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("gun_1_jntSocket"));

}

// Called when the game starts or when spawned
void AAITank_1::BeginPlay()
{
	Super::BeginPlay();
	// Tank physics settings
	BodyMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
	TracksMaterial = GetMesh()->CreateDynamicMaterialInstance(1);
}

// Called every frame
void AAITank_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bStopTurn = (GetMesh()->GetPhysicsAngularVelocityInDegrees().Length() > 30);
	SetSpeed();
}

void AAITank_1::Move(float value)
{

	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();

	if (VehicleMovement == nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("VehicleMovement is NULL"));
	}
	MoveState = value;
	
	if (value > 0.f) {
		VehicleMovement->SetThrottleInput(value);
		VehicleMovement->SetBrakeInput(0.f);
	}
	else
	{
		VehicleMovement->SetThrottleInput(value);
		VehicleMovement->SetBrakeInput(-value);
	}
	
}

void AAITank_1::BodyTurn(float value)
{
	
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	if (value != 0) {
		if (bStopTurn)
		{
			VehicleYaw = FMath::Lerp(VehicleYaw, 0.f, 1.f);
			VehicleMovement->SetYawInput(VehicleYaw);
		}
		else
		{
			if (FVector::DotProduct(GetMesh()->GetPhysicsLinearVelocity(), GetActorForwardVector()) < 0)
			{
				VehicleYaw = -value;
			}else
			{
				VehicleYaw = value;
			}

			VehicleMovement->SetYawInput(VehicleYaw);
		}
	}
	if (value != 0 && MoveState == 0)
	{
		Move(0.2f);
	}
	if(value == 0)
	{
		VehicleMovement->SetYawInput(0.f);
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

void AAITank_1::RotateTurret(FVector ToTarget)
{
	// To Target의 방향으로 돌려주면 되는데, 그 기준을 0으로 만들어야했음..
	float ToAngle = -(GetActorRotation().Yaw - ToTarget.Rotation().Yaw);
	if (ToAngle >= -300 && ToAngle < -295)
	{
		TurretAngle = ToAngle;
	}
	else if (ToAngle > 55 && ToAngle <= 60)
	{
		TurretAngle =  ToAngle;
	}
	else
	{
		TurretAngle = FMath::Lerp(TurretAngle,ToAngle,1.f);
	}

	SetHead();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

}

void AAITank_1::RotateBarrel(FVector LookAtTarget)
{
	// 포신 각도 설정 : 탄도학 적용 버전
	FVector ToTarget = LookAtTarget - GetMesh()->GetSocketLocation(FName("turret_jnt"));
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	float TargetDistance = ToTarget.Size2D() / 100.f; // 수평 거리
	float TargetHeight = ToTarget.Z / 100.f; // 높이 차이
	LookAtRotation.Pitch = CalculateLaunchAngle(FireSpeed, TargetDistance, TargetHeight);
	LookPitch = LookAtRotation.Pitch;
	// 포신 각도 제한
	if (LookAtRotation.Pitch < DownLimit) {
		LookAtRotation.Pitch = DownLimit;
	}
	if (LookAtRotation.Pitch > UpLimit) {
		LookAtRotation.Pitch = UpLimit;
	}

	float ToAngle = -(GetActorRotation().Pitch - LookAtRotation.Pitch);
	GunElevation = ToAngle;

	SetGun();
}

float AAITank_1::RotateTank(FVector LookAtTarget)
{	
	
	if (LookAtTarget == FVector::ZeroVector) {
		return 0.f;
	}
	FRotator CurrentRotation = GetMesh()->GetComponentRotation();
	FVector ToTarget = LookAtTarget - GetMesh()->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	// 몸체 돌리기
	float YawDifference = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, LookAtRotation.Yaw);

	float ReturnValue = 0.f;
	if (IsRotationCompleted(CurrentRotation, ToTarget.Rotation(),10.f))
	{
		ReturnValue = 0.f;
	}
	else {
		if (YawDifference > 0)
		{
			ReturnValue = 1.f;
		}
		else
		{
			ReturnValue = -1.f;
		}
	}
	return ReturnValue;
}

bool AAITank_1::IsRotationCompleted(FRotator CurrentRotation, FRotator TargetRotation, float Tolerance)
{
	float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
	return YawDifference <= Tolerance;
}

float AAITank_1::CalculateLaunchAngle(float LaunchSpeed, float TargetDistance, float TargetHeight)
{
	const float Gravity = 9.8f;
	float LaunchSpeedSquared = LaunchSpeed * LaunchSpeed;
	float Term1 = LaunchSpeedSquared * LaunchSpeedSquared - Gravity * (Gravity * TargetDistance * TargetDistance + 2 * TargetHeight * LaunchSpeedSquared);

	if (Term1 < 0)
	{
		return 0.0f;
	}

	Term1 = sqrt(Term1);
	float Term2 = Gravity * TargetDistance;
	
	float Angle1 = atan((LaunchSpeedSquared + Term1) / Term2);
	float Angle2 = atan((LaunchSpeedSquared - Term1) / Term2);

	// 선택: 두 개의 각도 중 하나를 선택 (낮은 각도 또는 높은 각도)
	return FMath::RadiansToDegrees(Angle2); //
}

void AAITank_1::SetSpeed()
{
	UAITANKAnim* AnimInstance = Cast<UAITANKAnim>(GetMesh()->GetAnimInstance());
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	if (VehicleMovement == nullptr)
	{
		return;
	}
	float speed = FMath::Clamp(VehicleMovement->GetForwardSpeed(),-500,500);
	AnimInstance->SetWheelSpeed(speed);
	return;
}

void AAITank_1::SetHead()
{
	if (GetMesh() == nullptr)
	{
		return;
	}
	if (UAITANKAnim* AnimInstance = Cast<UAITANKAnim>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetTurretAngle(TurretAngle);
	}
}

void AAITank_1::SetGun()
{
	if (GetMesh() == nullptr)
	{
		return;
	}
	if (UAITANKAnim* AnimInstance = Cast<UAITANKAnim>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetGunElevation(GunElevation);
	}
}
