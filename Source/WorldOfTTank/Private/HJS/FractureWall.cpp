// Fill out your copyright notice in the Description page of Project Settings.

#include "TimerManager.h"
#include "HJS/FractureWall.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
AFractureWall::AFractureWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FracturedMeshComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("FracturedMeshComponent"));
	RootComponent = FracturedMeshComp;	

}

// Called when the game starts or when spawned
void AFractureWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFractureWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AFractureWall::HpDown()
{
	WallHP--;

	if (WallHP > 0) {
		return true;
	}
	else {
		return false;
	}
}

void AFractureWall::SetDestroyTimer()
{
	if(!bBroken){
		bBroken = true;
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AFractureWall::DestroySelf, 5.0f, false);
	}
}

void AFractureWall::DestroySelf()
{
	Destroy();
}