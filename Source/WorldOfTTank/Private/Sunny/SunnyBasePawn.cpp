// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyBasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"


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


//void ASunnyBasePawn::Fire()
//{
//	FVector Location = ProjectileSpawnPoint->GetComponentLocation();
//	FRotator Rotation = ProjectileSpawnPoint->GetComponentRotation();
//
//}
