// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyRealTankPawn.h"
#include "Sunny/SunnyProjectile.h"
#include "Sunny/SunnyVehicleAnimationInstance.h"
#include "Sunny/SunnyAIController.h"

#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "Components/WidgetComponent.h"
#include "Components/progressBar.h"
#include "Components/TextBlock.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "ChaosWheeledVehicleMovementComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"





ASunnyRealTankPawn::ASunnyRealTankPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(GetMesh());


	// Projectile 발사 위치
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(GetMesh(), FName("gun_jntSocket"));

	// Fire 이펙트 나이아가라 컴포넌트 추가
	FireNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireNiagara"));
	FireNiagara->SetupAttachment(ProjectileSpawnPoint);
	FireNiagara->bAutoActivate = false; // 처음에는 비활성 상태로 설정
	FireNiagara->SetRelativeScale3D(FVector(1.f)); // 필요에 따라 크기 조정

	// Track 사운드 컴포넌트 추가
	TrackSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TrackSound"));
	TrackSoundComp->SetupAttachment(RootComponent);
	TrackSoundComp->bAutoActivate = false;

	FireSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FireSound"));
	FireSoundComp->SetupAttachment(RootComponent);
	FireSoundComp->bAutoActivate = false;
}

void ASunnyRealTankPawn::BeginPlay()
{
	Super::BeginPlay();

	// Tank physics settings
	BodyMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
	TracksMaterial = GetMesh()->CreateDynamicMaterialInstance(1);

	// Track 사운드 설정
	if (TrackSound)
	{
		TrackSoundComp->SetSound(TrackSound);
	}

	if (FireSound)
		FireSoundComp->SetSound(FireSound);

}



void ASunnyRealTankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	bStopTurn = (GetMesh()->GetPhysicsAngularVelocityInDegrees().Length() > 30);
	SetSpeed();
	UpdateTrackSound();
}



void ASunnyRealTankPawn::SetSpeed()
{
	USunnyVehicleAnimationInstance* SunnyAiAnim = Cast<USunnyVehicleAnimationInstance>(GetMesh()->GetAnimInstance());
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	if (nullptr == VehicleMovement)
	{
		return;
	}
	float speed = FMath::Clamp(VehicleMovement->GetForwardSpeed(), -500, 500);
	SunnyAiAnim->SetWheelSpeed(speed);
}

void ASunnyRealTankPawn::UpdateTrackSound()
{
	FVector velocity = GetMesh()->GetPhysicsLinearVelocity();
	float speed = velocity.Size();

	// if (speed > 0.0f) {
		if (!TrackSoundComp->IsPlaying())
		{
			TrackSoundComp->Play();
		}

		float Pitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1000.f), FVector2D(0.5f, 1.5f), speed);
		TrackSoundComp->SetPitchMultiplier(Pitch);
	// }
	/*else
	{
		if (TrackSoundComp->IsPlaying())
		{
			TrackSoundComp->Stop();
		}
	}*/
}



// 탱크 몸체 돌리기
float ASunnyRealTankPawn::CheckTankBodyRotation(FVector TurnToTarget)
{
	// 아주 간단한 탱크 몸체 회전 방법
	/*UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	if (nullptr == VehicleMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO Get VehicleMovement"));
		return;
	}

	FVector foward = GetActorForwardVector();
	FVector lookDir = LookAtTarget - GetActorLocation();
	lookDir.Normalize();
	FVector right = GetActorRightVector();
	float dot = FVector::DotProduct(lookDir, right);
	VehicleMovement->SetYawInput(dot);*/


	if (TurnToTarget == FVector::ZeroVector) {
		return 0.f;
	}

	// 현재 메쉬의 오른쪽 벡터와 앞쪽 벡터 정규화
	FVector RightVector = GetMesh()->GetRightVector().GetSafeNormal();
	FVector ForWordVector = GetMesh()->GetForwardVector().GetSafeNormal();

	// 목표 위치에서 현재 메쉬 위치를 뺀 벡터를 계산하여 direction에 저장
	FVector direction = TurnToTarget - GetMesh()->GetComponentLocation();
	direction.Z = 0.f;
	// 목표 벡터를 정규화
	direction.Normalize();

	// RightVector와 direction 간의 내적 계산
	float result = FVector::DotProduct(RightVector, direction);
	// ForWordVector와 direction 간의 내적 계산
	float result2 = FVector::DotProduct(ForWordVector, direction);

	// 디버깅 로그 추가
	//UE_LOG(LogTemp, Warning, TEXT("RightVector: %s, ForWordVector: %s, direction: %s"), *RightVector.ToString(), *ForWordVector.ToString(), *direction.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("result: %f, result2: %f"), result, result2);

	if ((result > -0.1f && result < 0.1f) && (result2 > 0.f)) {
		return 0.f;
	}
	else {
		return result > 0 ? 1.f : -1.f; 
	}

	// 디버깅 로그 추가
	//UE_LOG(LogTemp, Warning, TEXT("Calculated rotation value: %f"), value);
}


// 탱크 머리 돌리기
void ASunnyRealTankPawn::RotateTurret(FVector lookAtTarget)
{
	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (nullptr == SkelMesh)
	{
		return;
	}

	// 타겟 방향 벡터 구하기
	FVector ToTarget = lookAtTarget - SkelMesh->GetSocketLocation(FName("turret_jnt"));


	/*if (!bTargetFound)
	{*/

	// 현재 탱크의 Yaw 회전과 목표 위치의 Yaw 회전 차이를 계산
	float angle = -(GetActorRotation().Yaw - ToTarget.Rotation().Yaw);
	//UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), angle);
	
	// 애니메이션 인스턴스를 가져옴
	USunnyVehicleAnimationInstance* SunnyAiAnim = Cast< USunnyVehicleAnimationInstance>(GetMesh()->GetAnimInstance());
	
	// 현재 포탑의 회전을 가져옴
	FRotator start = SunnyAiAnim->GetTurretRotation();
	//UE_LOG(LogTemp, Warning, TEXT("start: %s"), *start.ToString());

	// 목표 위치로의 회전 값을 설정 (Yaw 값만 사용)
	FRotator end = FRotator(0, angle, 0);
	//UE_LOG(LogTemp, Warning, TEXT("end: %s"), *end.ToString());

	// 현재 회전에서 목표 회전으로 일정 속도로 회전
	FRotator lerp = FMath::RInterpConstantTo(
		start,
		end,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		35);

	// 계산된 회전 값을 애니메이션 인스턴스에 설정
	SunnyAiAnim->SetTurretRotation(lerp);

	// 목표 위치의 회전 값을 설정
	FRotator lookAtRotation = FRotator(0.f, lookAtTarget.Rotation().Yaw, 0.f);
	//UE_LOG(LogTemp, Warning, TEXT("LookAtRotation: %s"), *lookAtRotation.ToString());

	float gab = end.Yaw - start.Yaw;
	//UE_LOG(LogTemp, Warning, TEXT("gab: %f"), gab);

	if (0.1 <= gab || -0.1 <= gab)
	{
		bTargetFound = true;
		//UE_LOG(LogTemp, Warning, TEXT("bTargetFound = true"));
	}

	//}

}


void ASunnyRealTankPawn::RotateTank(float value)
{
	//UChaosVehicleMovementComponent 가져오기
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	if (nullptr == VehicleMovement)
	{
		//UE_LOG(LogTemp, Warning, TEXT("NO Get VehicleMovement"));
		return;
	}

	if (value != 0) {

		if (bStopTurn)
		{
			// VehicleYaw를 0으로 선형 보간하고, VehicleMovement의 YawInput 설정
			VehicleYaw = FMath::Lerp(VehicleYaw, 0.f, 1.f);
			VehicleMovement->SetYawInput(VehicleYaw);
			//UE_LOG(LogTemp, Warning, TEXT("Setting YawInput: %f"), VehicleYaw);
		}
		else
		{
			VehicleYaw = value;
			VehicleMovement->SetYawInput(VehicleYaw);
			//UE_LOG(LogTemp, Warning, TEXT("2. Setting YawInput: %f"), VehicleYaw);
		}
	}
	else if (value == 0)
	{
		// 회전 값이 0인 경우, VehicleMovement의 YawInput을 0으로 설정
		VehicleMovement->SetYawInput(0.f);
	}


	// 회전 값이 0이 아니고 MoveState가 0인 경우에 Move를 호출합니다.
	/*if (value != 0 && MoveState == 0)
	{
		Move(1.0f);
	}*/
}

// 타켓 방향으로 탱크 움직이기
void ASunnyRealTankPawn::Move(float value)  
{
	//UE_LOG(LogTemp, Warning, TEXT("Move()"));
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	//UE_LOG(LogTemp, Warning, TEXT("%f"), value);
	MoveState = value;
	if (VehicleMovement == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("VehicleMovement is NULL"));
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("move value : %f"), value);

	if (value > 0.f) {
		//UE_LOG(LogTemp, Warning, TEXT("Setting throttle to %f"), value);
		VehicleMovement->SetThrottleInput(value);
		VehicleMovement->SetBrakeInput(0.f);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Setting brake to %f"), -1 * value);
		VehicleMovement->SetThrottleInput(value);
		VehicleMovement->SetBrakeInput(-1.f * value);
	}
	return;

}


void ASunnyRealTankPawn::Fire()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire()"));
	// 총알스폰점에 총알 생성 위치 + 방향 할당
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	ASunnyProjectile* Projectile = GetWorld()->SpawnActor<ASunnyProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);

	if (FireNiagara) {
		// ProjectileSpawnPoint의 위치와 회전에 Niagara 이펙트 재생
		FireNiagara->SetWorldLocationAndRotation(Location, Rotation);
		FireNiagara->Activate(); // Niagara 이펙트 활성화
	}
	FireSoundComp->Play(0.f);


}

void ASunnyRealTankPawn::HandleDestruction()
{
}


UAudioComponent* ASunnyRealTankPawn::GetTrackSoundComp() const
{
	return TrackSoundComp;
}