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
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "PaperSpriteComponent.h"

// Sets default values
AAITank_1::AAITank_1()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(GetMesh());

	// 스폰 포인트 생성
	ProjecttileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("AITank_1 Spawn Point"));
	ProjecttileSpawnPoint->SetupAttachment(GetMesh(), FName("gun_1_jntSocket"));

	TrackSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudioComponent"));
	TrackSoundComp->SetupAttachment(RootComponent);
	TrackSoundComp->bAutoActivate = false;

	FireSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSoundComponent"));
	FireSoundComp->SetupAttachment(RootComponent);
	FireSoundComp->bAutoActivate = false; 

	// Enemy Indicator 컴포넌트 추가
	Indicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Indicator"));
	Indicator->SetupAttachment(RootComponent);

	//OutlineMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("OutlineMesh"));
	//OutlineMesh->SetupAttachment(RootComponent);

	//if (GetMesh()->SkeletalMesh)
	//{
	//	OutlineMesh->SkeletalMesh = GetMesh()->SkeletalMesh;
	//}

	//if (GetMesh()->GetAnimClass())
	//{
	//	OutlineMesh->SetAnimInstanceClass(GetMesh()->GetAnimClass());
	//}

}

// Called when the game starts or when spawned
void AAITank_1::BeginPlay()
{
	Super::BeginPlay();
	// Tank physics settings
	BodyMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
	TracksMaterial = GetMesh()->CreateDynamicMaterialInstance(1);
	if (TrackSoundCue)
	{
		TrackSoundComp->SetSound(TrackSoundCue);
	}

	if (FireSound)
	{
		FireSoundComp->SetSound(FireSound);
	}
	//OnOutLine();
	//OffOutLine();
}

// Called every frame
void AAITank_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bStopTurn = (GetMesh()->GetPhysicsAngularVelocityInDegrees().Length() > 40);
	SetSpeed();
	UpdateMovementSound();
}

void AAITank_1::Move(float value)
{

	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();

	if (VehicleMovement == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("VehicleMovement is NULL"));
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
			}
			else
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
	if (value == 0)
	{
		VehicleMovement->SetYawInput(0.f);
	}
}

void AAITank_1::Fire()
{
	if (GunFire)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		GunFire,
		ProjecttileSpawnPoint->GetComponentLocation(),
		ProjecttileSpawnPoint->GetComponentRotation());
	}

	if (FireSound && FireSoundComp)
	{
		FireSoundComp->Play(0.f);
	}

	FVector ProjecttileSpawnPointLocation = ProjecttileSpawnPoint->GetComponentLocation();
	FRotator ProjecttileSpawnPoinRotation = ProjecttileSpawnPoint->GetComponentRotation();
	if (ProjecttileClass) {
		AAIProjecttile_1* Projectile = GetWorld()->SpawnActor<AAIProjecttile_1>(ProjecttileClass, ProjecttileSpawnPointLocation, ProjecttileSpawnPoinRotation);
		Projectile->SetOwner(this);
	}
}

void AAITank_1::RotateTurret(FVector ToTarget)
{
	// To Target의 방향으로 돌려주면 되는데, 그 기준을 0으로 만들어야했음..
	float ToAngle = -(GetActorRotation().Yaw - ToTarget.Rotation().Yaw);

	FRotator ToAngleRot(0.f, ToAngle, 0.f);
	FRotator TurretAngleRot(0.f, TurretAngle, 0.f);

	TurretAngle = FMath::RInterpConstantTo(TurretAngleRot, ToAngleRot, GetWorld()->GetDeltaSeconds(), 60.f).Yaw;
	SetHead();

}

void AAITank_1::RotateBarrel(FVector LookAtTarget)
{
	// 포신 각도 설정 : 탄도학 적용 버전
	// 타겟의 로테이션이 바닥을 기준으로 되어있어 조금 올림
	LookAtTarget.Z = LookAtTarget.Z;
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
	FVector RightVector = GetMesh()->GetRightVector().GetSafeNormal();
	FVector ForWardVector = GetMesh()->GetForwardVector().GetSafeNormal();
	//FRotator CurrentRotation = GetMesh()->GetComponentRotation();
	FVector ToTarget = LookAtTarget - GetMesh()->GetComponentLocation();
	ToTarget.Z = 0.f;
	ToTarget.Normalize();
	float result = FVector::DotProduct(RightVector, ToTarget);
	float result2 = FVector::DotProduct(ForWardVector, ToTarget);
	//UE_LOG(LogTemp, Warning, TEXT("RightVector Dot : %f"), result);
	//UE_LOG(LogTemp, Warning, TEXT("FordardVector Dot : %f"), result2);
	if ((result > -0.1f && result < 0.1f) && (result2 > 0.9f))
		return 0;
	// Right 양 Forward양일때
	if (result > 0.f && result2 > 0.f)
	{
		return 1.f;
	// Right 양, Forward 음일때
	}
	else if (result > 0.f && result2 < 0.f)
	{
		return 1.f;
	}
	// 나머지 ( 음음, 음양 , 0)
	else
	{
		return -1.f;
	}
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

void AAITank_1::UpdateMovementSound()
{
	FVector Velocity = GetMesh()->GetPhysicsLinearVelocity();
	float Speed = Velocity.Size();

	if (Speed > 0.0f){
		if (!TrackSoundComp->IsPlaying())
		{
			TrackSoundComp->Play();
		}

		float Pitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f,1000.f), FVector2D(0.5f,1.5f),Speed);
		TrackSoundComp->SetPitchMultiplier(Pitch);
	}
	else
	{
		if (TrackSoundComp->IsPlaying())
		{
			TrackSoundComp->Stop();
		}
	}

}

void AAITank_1::SetSpeed()
{
	UAITANKAnim* AnimInstance = Cast<UAITANKAnim>(GetMesh()->GetAnimInstance());
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	if (VehicleMovement == nullptr)
	{
		return;
	}
	float speed = FMath::Clamp(VehicleMovement->GetForwardSpeed(), -500, 500);
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

void AAITank_1::OnOutLine()
{
	GetMesh()->bRenderCustomDepth = true;
}

void AAITank_1::OffOutLine()
{
	GetMesh()->bRenderCustomDepth = false;
}
