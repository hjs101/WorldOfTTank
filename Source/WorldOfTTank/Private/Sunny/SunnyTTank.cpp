// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyEnemyFSM.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Line Trace 구현 때 필요한 헤더파일
#include "Engine/World.h" 
#include "DrawDebugHelpers.h"   // 라인트레이스 시각화
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"



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

	TTankPlayerController = Cast<APlayerController>(GetController());
}

void ASunnyTTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TTankPlayerController)
	{
		FHitResult HitResult;
		TTankPlayerController->GetHitResultUnderCursor(
			ECollisionChannel::ECC_Visibility,
			false,
			HitResult);

		// 디버그 구체 만드는 법
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 25.f, 12, FColor::Red, false, -1.f);

		RotateTurret(HitResult.ImpactPoint);


	}
}

//Input 매핑에 바인딩
void ASunnyTTank::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASunnyTTank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASunnyTTank::Turn);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ASunnyTTank::Fire);
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

// TTank Hide
void ASunnyTTank::HandleDestruction()
{
	Super::HandleDestruction();

	// Hide
	SetActorHiddenInGame(true);

	// Tick DIsable
	SetActorTickEnabled(false);

}



// 필요 없음!!!!
//void  ASunnyTTank::CheckEnemy()
//{
//
//	// LineTrace의 시작 위치
//	FVector startPos = this->GetActorLocation();
//	// LineTrace의 종료 위치
//	//FVector end = (TTankPlayerController->GetControlRotation().Vector() * LineDistance) + StartPos;
//	FVector endPos = startPos + this->GetActorForwardVector() * LineDistance;
//
//	//LineTrace의 충돌 정보를 담을 변수
//	FHitResult hitInfo;
//
//	// 충돌 옵션 설정 변수
//	FCollisionQueryParams params;
//
//	// 자기 자신(플레이어)는 충돌에서 제외
//	params.AddIgnoredActor(this);
//
//	// Channel 필터를 이용한 충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
//	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//
//	// LineTrace가 부딪혔을 때
//	if (bHit)
//	{
//
//		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
//		if (enemy)
//		{
//			auto EnemyFSM = Cast<USunnyEnemyFSM>(enemy);
//			EnemyFSM->OnDamageProcess();
//		}
//	}
//}