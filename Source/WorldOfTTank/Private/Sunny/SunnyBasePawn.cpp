// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyBasePawn.h"
#include "Sunny/SunnyProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASunnyBasePawn::ASunnyBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	BodyMesh->SetupAttachment(CapsuleComp);

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head Mesh"));
	HeadMesh->SetupAttachment(BodyMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(HeadMesh);
}

// 터렛 회전
void ASunnyBasePawn::RotateTurret(FVector LookAtTarget)
{
	FVector ToTarget = LookAtTarget - HeadMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	HeadMesh->SetWorldRotation(
		FMath::RInterpTo(
			HeadMesh->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this),
			10.f)
	);
}

// 발사
void ASunnyBasePawn::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire()"));
	// 총알스폰점에 총알 생성 위치 + 방향 할당
	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();

	ASunnyProjectile* Projectile = GetWorld()->SpawnActor<ASunnyProjectile>(ProjectileClass, Location, Rotation);
	Projectile->SetOwner(this);

	// 디버그 구체로 확인
	//DrawDebugSphere(GetWorld(), ProjectileSpawnPointLocation, 25.f, 12, FColor::Green, false, 3.f);		
}


// 폰 파괴
void ASunnyBasePawn::HandleDestruction()
{
	// 할일 : 비주얼 사운드 
}