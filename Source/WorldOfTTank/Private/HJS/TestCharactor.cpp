// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/TestCharactor.h"

// Sets default values
ATestCharactor::ATestCharactor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestCharactor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestCharactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestCharactor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

