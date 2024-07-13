// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyNewFSM.h"
#include "Sunny/SunnyNewTTank.h"

#include "CSW/PlayerTankVehicle.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"


// Sets default values for this component's properties
USunnyNewFSM::USunnyNewFSM()
{
	// 초기화 시 Null 포인터 접근을 방지하기 위해 Owner 확인
	if (GetOwner() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("USunnyEnemyFSM constructor called with null owner!"));
		return;
	}

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//UE_LOG(LogTemp, Warning, TEXT("USunnyEnemyFSM created for %s"), *GetOwner()->GetName());
}


// Called when the game starts
void USunnyNewFSM::BeginPlay()
{
	Super::BeginPlay();

	// 월드에서 APlayerTank 타깃 찾아오기
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerTankVehicle::StaticClass());
	// APlayerTank 타입으로 캐스팅
	Target = Cast<APlayerTankVehicle>(actor);
	// 소유 객체 가져오기
	Me = Cast<ASunnyNewTTank>(GetOwner());

	// AAiController 할당
	AiController = Cast<AAIController>(Me->GetController());
	
}


// Called every frame
void USunnyNewFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 메세지 출력하기
	FString logMsg = UEnum::GetValueAsString(SunnyAiState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (SunnyAiState)
	{
	case ESunnyAiState::Idle:
		IdleState();
		break;
	case ESunnyAiState::Move:
		MoveState();
		break;
	case ESunnyAiState::Attack:
		AttackState();
		break;
	case ESunnyAiState::Die:
		DieState();
		break;
	}
}


bool USunnyNewFSM::GetRandomPositionInNavMesh(FVector centerLocaion, float radius, FVector& dest)
{
	FNavLocation NavLocation;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys && NavSys->GetRandomPointInNavigableRadius(centerLocaion, radius, NavLocation))
	{
		dest = NavLocation.Location;
		return true;
	}
	return false;
}



void USunnyNewFSM::IdleState()
{
	UE_LOG(LogTemp, Warning, TEXT("IdleState()"));

	//1. 시간이 흘렀으니까
	CurrentTime += GetWorld()->DeltaTimeSeconds;

	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (CurrentTime > IdleDelayTime)
	{
		// 3. 이동 상태로 전환하고 싶다
		SunnyAiState = ESunnyAiState::Move;
		UE_LOG(LogTemp, Warning, TEXT("Change IdleState->MoveState"));
		// 경과 시간 초기화
		CurrentTime = 0;

		//최초 램덤한 위치 정해주기
		GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
	}
}


void USunnyNewFSM::MoveState()
{
	UE_LOG(LogTemp, Warning, TEXT("MoveState()"));

	FVector destination;
	// 타깃이 존재하는지 확인
	if (Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Found"));
		destination = Target->GetActorLocation();

		if (nullptr != Me)
		{
			UE_LOG(LogTemp, Warning, TEXT("Rotate Turret"));
			Me->CalcTurretRotation(Target);
		}
		UE_LOG(LogTemp, Warning, TEXT("Rotate Tank"));
		Me->RotateTank(destination);
	}
	else
	{
		// 타깃이 없다면 Idle 상태로 전환
		SunnyAiState = ESunnyAiState::Idle;
		UE_LOG(LogTemp, Warning, TEXT("Change MoveState->IdleState"));
		return;
	}

	// NavigationSystem 객체 얻어오기
	auto ns = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!ns)
	{
		UE_LOG(LogTemp, Error, TEXT("NavigationSystem is null in USunnyEnemyFSM::MoveState"));
		return;
	}

	// 목적지 길 찾기 경로 데이터 검색
	FPathFindingQuery query;
	FAIMoveRequest req;
	// 목적지에서 인지할 수 있는 범위
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);
	// 길 찾기를 위한 쿼리 생성
	AiController->BuildPathfindingQuery(req, query);
	// 길 찾기 결과 가져오기
	FPathFindingResult r = ns->FindPathSync(query);
	// 목적지까지의 길 찾기 성공 여부 확인
	if (r.Result == ENavigationQueryResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("ENavigationQueryResult::Success"));
		
		// 타깃쪽으로 이동
		AiController->MoveToLocation(destination);
		UE_LOG(LogTemp, Warning, TEXT("AiController->MoveToLocation(destination)"));

		/*if (nullptr != Me)
		{
			UE_LOG(LogTemp, Warning, TEXT("Move Sunny Ai TTank"));
			Me->Move(1);
			UE_LOG(LogTemp, Warning, TEXT("Move Forward"));
		}*/
	}
	else
	{
		// 랜덤 위치로 이동
		auto result = AiController->MoveToLocation(RandomPos);
		// 목적지에 도착하면
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// 새로운 랜덤 위치 가져오기
			GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
		}
	}

	if (nullptr != Me)
	{
		UE_LOG(LogTemp, Warning, TEXT("Move Sunny Ai TTank"));
		Me->Move(1);
		UE_LOG(LogTemp, Warning, TEXT("Move Forward"));
	}


	// 타깃과 가까워지면 공격 상태로 전환하고 싶다
	// 1. 만약 거리가 공격 범위 안에 들어오면
	if (FVector::Dist(destination, Me->GetActorLocation()) <= AttackRange)
	{
		// 길찾기 기능 정지
		AiController->StopMovement();

		// 2. 공격 상태로 전환하고 싶다
		SunnyAiState = ESunnyAiState::Attack;

		if (nullptr != Me)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack Sunny Ai TTank"));
			Me->EnemyMove->StopMovementImmediately();
			Me->Move(0);
			UE_LOG(LogTemp, Warning, TEXT("Stop Move"));
		}
	}
}

void USunnyNewFSM::AttackState()
{
	UE_LOG(LogTemp, Warning, TEXT("AttackState()"));

	if (isTimerSeted == false)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Attack!!!!"));
		Me->SetFireTimer();
		isTimerSeted = true;
	}

	// 목표 : 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다
	// 1. 타깃과의 거리가 필요하다
	float Distance = 0.0f;
	if (Target)
	{
		Distance = FVector::Distance(Target->GetActorLocation(), Me->GetActorLocation());
	}
	// 2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (Distance > AttackRange)
	{
		// 3. 상태를 이동으로 전환하고 싶다
		SunnyAiState = ESunnyAiState::Idle;
		Me->ClearFIreTimer();
		isTimerSeted = false;

		// 최초 램덤한 위치 정해주기
		GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
	}
}

void USunnyNewFSM::DieState()
{

}