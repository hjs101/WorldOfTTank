// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyRealTankPawn.h"
#include "Sunny/SunnyProjectile.h"
#include "Sunny/SunnyVehicleAnimationInstance.h"

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

#include "Kismet/GameplayStatics.h"



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

}

void ASunnyRealTankPawn::BeginPlay()
{
	Super::BeginPlay();

	// Tank physics settings
	BodyMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
	TracksMaterial = GetMesh()->CreateDynamicMaterialInstance(1);
}

void ASunnyRealTankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//SetSpeed();

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



// 탱크 몸체 돌리기
void ASunnyRealTankPawn::RotateTank(FVector LookAtTarget)
{
	//UE_LOG(LogTemp, Warning, TEXT("RotateTank()"));

	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	
	int value = 0;
	if (FVector::DotProduct(GetMesh()->GetPhysicsLinearVelocity(), GetActorForwardVector()) < 0)
	{
		VehicleYaw = -1;
	}
	else
	{
		VehicleYaw = 1;
	}

	VehicleMovement->SetYawInput(VehicleYaw);

	/*if (value != 0 && MoveState == 0)
	{
		Move(0.2f);
	}
	if (value == 0)
	{
		VehicleMovement->SetYawInput(0.f);
	}*/

}


// 탱크 머리 돌리기
void ASunnyRealTankPawn::RotateTurret(FVector LookAtTarget)
{
	//UE_LOG(LogTemp, Warning, TEXT("RotateTurret()"));
	if (nullptr == GetMesh())
	{
		return;
	}

	if (!bFound)
	{
		bFound = true;
	}

	if (bFound)
	{ 
		// 현재 탱크의 Yaw 회전과 목표 위치의 Yaw 회전 차이를 계산
		float Angle = -(GetActorRotation().Yaw - LookAtTarget.Rotation().Yaw);
	
		// 애니메이션 인스턴스를 가져옴
		USunnyVehicleAnimationInstance* SunnyAiAnim = Cast< USunnyVehicleAnimationInstance>(GetMesh()->GetAnimInstance());
	
		// 현재 포탑의 회전을 가져옴
		FRotator start = SunnyAiAnim->GetTurretRotation();

		// 목표 위치로의 회전 값을 설정 (Yaw 값만 사용)
		FRotator end = FRotator(0, Angle, 0);

		// 현재 회전에서 목표 회전으로 일정 속도로 회전
		FRotator lerp = FMath::RInterpConstantTo(
			start,
			end,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			35);

		// 계산된 회전 값을 애니메이션 인스턴스에 설정
		SunnyAiAnim->SetTurretRotation(lerp);

		// 목표 위치의 회전 값을 설정
		FRotator LookAtRotation = FRotator(0.f, LookAtTarget.Rotation().Yaw, 0.f);
	}
	
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
		UE_LOG(LogTemp, Error, TEXT("VehicleMovement is NULL"));
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

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}


}

void ASunnyRealTankPawn::HandleDestruction()
{
}

