// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyEnemyFSM.h"
#include "Sunny/SunnyGameMode.h"

#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"


ASunnyEnemy::ASunnyEnemy()
{
	// EnemyFSM 컴포넌트 추가
	Fsm = CreateDefaultSubobject<USunnyEnemyFSM>(TEXT("FSM"));

	// Pawn Move 컴포넌트 추가
	EnemyMove = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
}

void ASunnyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Find the distance to the Tank
	//if (TTank)
	//{
	//	float Distance = FVector::Dist(GetActorLocation(), TTank->GetActorLocation());

	//	// Check to see if the Tank is in range
	//	if (Distance <= FireRange)
	//	{
	//		// If in range, rotate turret towrad Tank
	//		RotateTurret(TTank->GetActorLocation());
	//	}
	//}

	if (InFireRange())
	{
		RotateTurret(TTank->GetActorLocation());
	}
}

void ASunnyEnemy::BeginPlay()
{ 
	Super::BeginPlay();

	TTank = Cast<ASunnyTTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	/*TTankGameMode = Cast<ASunnyGameMode>(UGameplayStatics::GetGameMode(this));*/
}



//  발사 타이머 설정
void ASunnyEnemy::SetFireTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("SetFireTimer()"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ASunnyEnemy::CheckFireCondition, FireRate, true);
}

// 발사 타이머 해제
void ASunnyEnemy::ClearFIreTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("ClearFireTimer()"));
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}
	


// 발사할지 여부 확인
void ASunnyEnemy::CheckFireCondition()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckFireCondition()"));
	if (InFireRange())
	{
		Fire();
	}
}

// 사정 거리 여부 확인 함수
bool ASunnyEnemy::InFireRange()
{
	if (TTank)
	{
		float Distance = FVector::Dist(GetActorLocation(), TTank->GetActorLocation());

		if (Distance <= FireRange)
		{
			return true;
		}
	}

	return false;
}



void ASunnyEnemy::OnDie()
{
	// 머리 날리기
	HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	HeadMesh->SetSimulatePhysics(true);
	HeadMesh->AddImpulse(FVector(0, 0, 2000), NAME_None, true);

	// 종료
	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorTickEnabled(false);
	EnemyMove->StopMovementImmediately();
	ClearFIreTimer();
}



// Enemy Delete
void ASunnyEnemy::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}