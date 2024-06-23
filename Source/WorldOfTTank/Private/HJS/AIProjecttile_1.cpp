// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFrameWork/ProjectileMovementComponent.h"
#include "HJS/AIProjecttile_1.h"

// Sets default values
AAIProjecttile_1::AAIProjecttile_1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjecttileComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projecttile Mesh"));
	SetRootComponent(ProjecttileComp);

	ProjecttileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projecttile Movement"));
	ProjecttileMovement->InitialSpeed = 10.f;
	ProjecttileMovement->MaxSpeed = 50.f;
}

// Called when the game starts or when spawned
void AAIProjecttile_1::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIProjecttile_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

