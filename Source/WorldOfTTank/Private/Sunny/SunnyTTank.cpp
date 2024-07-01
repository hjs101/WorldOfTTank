// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyTTank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"

ASunnyTTank::ASunnyTTank()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ASunnyTTank::BeginPlay()
{
	Super::BeginPlay();

	TankPlayerController = Cast<APlayerController>(GetController());
}

void ASunnyTTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TankPlayerController)
	{
		FHitResult HitResult;
		TankPlayerController->GetHitResultUnderCursor(
			ECollisionChannel::ECC_Visibility,
			false,
			HitResult);

		// 디버그 구체 만드는 법
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 25.f, 12, FColor::Red, false, -1.f);

		RotateTurret(HitResult.ImpactPoint);
	}
}

void ASunnyTTank::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASunnyTTank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASunnyTTank::Turn);

	//PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ASunnyTTank::Fire);
}

void ASunnyTTank::Move(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value);

	FVector DeltaLocation = FVector::ZeroVector;  
	// X = Value * DeltaTime * Speed
	DeltaLocation.X = Value * Speed * UGameplayStatics::GetWorldDeltaSeconds(this);
	// 물체의 xyz방향에 따라 위치 이동
	AddActorLocalOffset(DeltaLocation, true);
}

void ASunnyTTank::Turn(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value);

	FRotator DeltaRotation = FRotator::ZeroRotator;
	// Yaw = Value * DeltaTime * TurnRate
	DeltaRotation.Yaw = Value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	// 물체의 xyz방향에 따라 회전
	AddActorLocalRotation(DeltaRotation, true);
}