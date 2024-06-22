// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/AITank_1.h"

// Sets default values
AAITank_1::AAITank_1()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called to bind functionality to input
void AAITank_1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

