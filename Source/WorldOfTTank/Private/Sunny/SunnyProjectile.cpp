// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASunnyProjectile::ASunnyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	/*ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f;*/

}

// Called when the game starts or when spawned
void ASunnyProjectile::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void ASunnyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

