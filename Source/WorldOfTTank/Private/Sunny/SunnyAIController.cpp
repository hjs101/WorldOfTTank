// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyAIController.h"
#include "Sunny/SunnyNewTTank.h"

#include "Sunny/SunnyNewFSM.h"

#include "CSW/PlayerTankVehicle.h"

#include "Kismet/GameplayStatics.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationData.h"


ASunnyAIController::ASunnyAIController()
{
	//PrimaryActorTick.bCanEverTick = true;

	FSM = CreateDefaultSubobject<USunnyNewFSM>(TEXT("FSM"));
	
}

void ASunnyAIController::BeginPlay()
{
	Super::BeginPlay();

	SunnyNewTTank = Cast<ASunnyNewTTank>(GetPawn());
	PlayerTTank = Cast<APlayerTankVehicle>(GetPawn());
	//FSM = Cast< USunnyNewFSM>(GetPawn())
}

void ASunnyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//MoveAlongPath(DeltaTime);
	//ASunnyNewTTank* sunnyAiTTank = Cast<ASunnyNewTTank>(GetPawn());
	//APlayerTankVehicle* playerTTank = Cast< APlayerTankVehicle>(GetPawn());
	//if (SunnyNewTTank)
	//{
	//	/*if (TurnState != 0)
	//	{
	//		MoveState = 0;
	//	};*/
	//	//SunnyAiTTank->Move(MoveState);
	//	//SunnyNewTTank->CalcTurretRotation(PlayerTTank);
	//}
}


void ASunnyAIController::FindTargetPath(FVector destination)
{
	// AI를 해당 위치로 이동
	//DrawDebugSphere(GetWorld(), AttackPosition, 50, 12, FColor::Green, false, 5.0f);
	if (nullptr != this)
	{
		//AIController->SetCurrentTask(this);

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		if (NavSys != nullptr)
		{
			FPathFindingQuery Query;
			FAIMoveRequest req;
			req.SetAcceptanceRadius(3);
			req.SetGoalLocation(destination);
			this->BuildPathfindingQuery(req, Query);

			FPathFindingResult Result = NavSys->FindPathSync(Query);

			if (Result.IsSuccessful())
			{
				this->SetNavPath(Result.Path);
			}
		}
	}
}


// 네비게이션 경로 설정 함수 (AI가 따라갈 경로 설정과 초기화)
void ASunnyAIController::SetNavPath(FNavPathSharedPtr Path)
{
	CurrentPath = Path;
	CurrentPathPointIndex = 0;
	CurrentTime = 0;
}


// 경로를 따라 이동 (AI 탱크를 캐스팅하고 이동 및 회전 상태 업데이트)
void ASunnyAIController::MoveAlongPath(float DeltaTime)
{
	// 현재 경로 존재 여부 확인
	if (CurrentPath)
	{
		// 현재 경로의 모든 경로 포인트 가져오기
		TArray<FNavPathPoint> PathPoints = CurrentPath->GetPathPoints();

		// 경로 포인트가 하나도 없으면 이동 종료
		if (PathPoints.Num() <= 0) {
			FinishMove(false);
		}

		// 경로 포인트가 하나 이상 있으면 경로따라 이동
		if (PathPoints.Num() > 1)
		{
			// 현재 위치 초기화
			FVector CurrentLocation = FVector::ZeroVector;

			// 목표 위치 초기화
			FVector TargetLocation = FVector::ZeroVector;

			// 현재 경로 포인트 인덱스가 경로 포인트 수보다 적으면 목표 위치 설정
			if (CurrentPathPointIndex < PathPoints.Num())
			{
				TargetLocation = PathPoints[CurrentPathPointIndex];
			}

			// 목표 위치의 Z값을 약간 높여서 설정
			TargetLocation.Z = TargetLocation.Z + 30.f;
			//DrawDebugSphere(GetWorld(),TargetLocation,50.f,12,FColor::Green,false,5.f);

			// AI 탱크가 유효하면 현재 위치와 상태를 업데이트
			if (SunnyNewTTank)
			{
				// 현재 위치를 터렛의 소켓 위치로 설정
				CurrentLocation = SunnyNewTTank->GetMesh()->GetSocketLocation(FName("turret_jnt"));

				// 이동 상태를 1로 설정
				//MoveState = 1;

				// 목표 위치를 향해 탱크를 회전시키고 회전 상태 업데이트
				//TurnState = SunnyNewTTank->RotateTank(TargetLocation);
				SunnyNewTTank->RotateTank(TargetLocation);

				// AI 탱크가 Attack 상태이고 FireRange이면 종료
				if (SunnyNewTTank->InFireRange())
				{
					FinishMove(true);
				}

				//// AI 탱크가 발사 상태이고 공격 모드이면 목표와의 거리 확인
				//if (SunnyNewTTank->GetFireState() && bAttackMode == true)
				//{

				//	//AActor* Target = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerTankVehicle::StaticClass());

				//	// 목표가 유효하면 목표와의 거리 확인
				//	if (Target != nullptr)
				//	{
				//		bool bNotAbleAttack = SunnyNewTTank->HasLineOfSightToTarget(CurrentLocation, Target);

				//		// 목표가 거리에 없다면 이동 종료
				//		if (!bNotAbleAttack)
				//		{
				//			FinishMove(true);
				//		}
				//	}
				//}
			}

			// 현재 위치가 목표 위치에 가까워지면 다음 결로 포인트로 이동
			if (FVector::Dist(CurrentLocation, TargetLocation) < AcceptanceRadius)
			{
				CurrentPathPointIndex++;

				// 모든 경로 포인트를 다 지나면 이동 완료
				if (CurrentPathPointIndex >= PathPoints.Num())
				{
					// 경로 완료
					FinishMove(true);
				}
			}
		}

		// 델타 시간을 현재 시간에 더하기
		CurrentTime += DeltaTime;

		// 현재 시간이 종료 시간을 초과하면 이동 종료
		if (CurrentTime > EndTime) {
			FinishMove(false);
		}
	}
	else {
		// 현재 경로가 없으면 경로 포인트 인덱스를 0으로 초기화
		CurrentPathPointIndex = 0;
	}
}


//  이동 완료 함수 (이동이 완료됐을 때 호출되며 현재 경로 및 관련 변수 초기화)
void ASunnyAIController::FinishMove(bool bSuccessed)
{
	CurrentPath = nullptr;
	CurrentPathPointIndex = 0;
	CurrentTime = 0;
	MoveState = 0;
	

	bNonStop = false;
	bAttackMode = false;
}


bool ASunnyAIController::GetRandomPositionInNavMesh(FVector centerLocaion, float radius, FVector& dest)
{
	// GetWorld()가 nullptr을 반환하는지 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld() returned nullptr."));
		return false;
	}

	// UNavigationSystemV1 객체 가져오기
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNavigationSystemV1::GetCurrent returned nullptr."));
		return false;
	}

	// 네비게이션 시스템에서 랜덤 위치 가져오기 시도
	FNavLocation NavLocation;
	if (NavSys->GetRandomPointInNavigableRadius(centerLocaion, radius, NavLocation))
	{
		dest = NavLocation.Location;
		return true;
	}

	// 실패한 경우 false 반환
	return false;
}

