// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/TestSunnyNewPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATestSunnyNewPawn::ATestSunnyNewPawn()
{
	// 스프링 암 생성
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("AITank_1 Spring Arm"));
	// 스프링 암을 루트 컴포넌트에 붙이기
	SpringArmComp->SetupAttachment(RootComponent);
	// 카메라 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("AITank_1 Camera"));
	// 카메라를 스프링 암에 붙이기
	CameraComp->SetupAttachment(SpringArmComp);
}

void ATestSunnyNewPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATestSunnyNewPawn::Move);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATestSunnyNewPawn::Fire);
}
