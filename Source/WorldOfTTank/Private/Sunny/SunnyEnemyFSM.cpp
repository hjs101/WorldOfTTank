// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyEnemyFSM.h"
#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyTTank.h"
//#include "Sunny/SunnyBasePawn.h"

#include "CSW/PlayerTank.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"



// Sets default values for this component's properties
USunnyEnemyFSM::USunnyEnemyFSM()
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

	UE_LOG(LogTemp, Warning, TEXT("USunnyEnemyFSM created for %s"), *GetOwner()->GetName());


	// 월드에서 APlayerTank 타깃 찾아오기
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerTank::StaticClass());
	// APlayerTank 타입으로 캐스팅
	Target = Cast<APlayerTank>(actor);
	// 소유 객체 가져오기
	Me = Cast<ASunnyEnemy>(GetOwner());

	// AAiController 할당
	Ai = Cast<AAIController>(Me->GetController());
}



// Called when the game starts
void USunnyEnemyFSM::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USunnyEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 메세지 출력하기
	//FString logMsg = UEnum::GetValueAsString(EnemyState);
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (EnemyState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	//case EEnemyState::Damage:
		//DamageState();
		//break;
	case EEnemyState::Die:
		DieState();
		break;		
	}
}


// 대기 상태
void USunnyEnemyFSM::IdleState()
{
	// 1. 시간이 흘렀으니까
	CurrentTime += GetWorld()->DeltaTimeSeconds;

	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (CurrentTime > IdleDelayTime)
	{
		// 3. 이동 상태로 전환하고 싶다
		EnemyState = EEnemyState::Move;
		// 경과 시간 초기화
		CurrentTime = 0;

		// 최초 램덤한 위치 정해주기
		GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
	}
}

// 이동 상태
void USunnyEnemyFSM::MoveState() 
{
	FVector destination;

	// 타깃이 존재하는지 확인
	if (Target)
	{
		destination = Target->GetActorLocation();
	}
	else
	{
		// 타깃이 없다면 Idle 상태로 전환
		EnemyState = EEnemyState::Idle;
		return;
	}

	// 방향 벡터 계산
	FVector dir = destination - Me->GetActorLocation();


	// NavigationSystem 객체 얻어오기
	//auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

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
	Ai->BuildPathfindingQuery(req, query);
	// 길 찾기 결과 가져오기
	FPathFindingResult r = ns->FindPathSync(query);
	// 목적지까지의 길 찾기 성공 여부 확인
	if (r.Result == ENavigationQueryResult::Success)
	{
		// 타깃쪽으로 몸을 돌리고 
		Me->RotateTank(Target->GetActorLocation());
		
		// 타깃쪽으로 이동
		Ai->MoveToLocation(destination);
	}
	else
	{
		// 랜덤 위치로 이동
		auto result = Ai->MoveToLocation(RandomPos);
		// 목적지에 도착하면
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// 새로운 랜덤 위치 가져오기
			GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
		}
	}

	// 타깃과 가까워지면 공격 상태로 전환하고 싶다
	// 1. 만약 거리가 공격 범위 안에 들어오면
	if (FVector::Dist(destination, Me->GetActorLocation()) <= AttackRange)
	{
		// 길찾기 기능 정지
		Ai->StopMovement();

		// 2. 공격 상태로 전환하고 싶다
		EnemyState = EEnemyState::Attack;
		Me->EnemyMove->StopMovementImmediately();
	}
}

// 공격 상태
void USunnyEnemyFSM::AttackState() 
{
	if (isTimerSeted == false)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Attack!!!!"));
		Me->SetFireTimer();
		isTimerSeted = true;
	}

	// 목표 : 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다
	// 1. 타깃과의 거리가 필요하다
	float Distance;

	if (Target)
	{
		Distance = FVector::Distance(Target->GetActorLocation(), Me->GetActorLocation());
	}
	// 2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (Distance > AttackRange)
	{
		// 3. 상태를 이동으로 전환하고 싶다
		EnemyState = EEnemyState::Move;
		Me->ClearFIreTimer();
		isTimerSeted = false;

		// 최초 램덤한 위치 정해주기
		GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
	}
}


// 피격 알림 이벤트 함수
//void USunnyEnemyFSM::OnDamageProcess()
//{
//	//UE_LOG(LogTemp, Warning, TEXT("Damage!!!!"));
//	Me->Destroy();
//
//	// 만약 체력이 남아있다면
//	if (hp > 0)
//	{
//		// 상태를 피격으로  전환
//		EnemyState = EEnemyState::Damage;
//	}
//	// 그렇지 않다면
//	else
//	{
//		// 상태를 죽음으로 전환
//		EnemyState = EEnemyState::Die;
//
//		// 캡슐 충돌체 비활성화
//		//Me->GetCapsuleComponent()->SetcollisionEnabled(ECollisionEnabled::NoCollision);
//	}
//}


// 피격 상태
void USunnyEnemyFSM::DamageState() 
{
	// 1. 시간이 흘렀으니까
	CurrentTime += GetWorld()->DeltaRealTimeSeconds;
	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (CurrentTime > DamageDelayTime)
	{
		//3. 대기 상태로 전환하고 싶다
		EnemyState = EEnemyState::Idle;
		// 경과 시간 초기화
		CurrentTime = 0;
	}
}

// 죽음 상태
void USunnyEnemyFSM::DieState() 
{
	UE_LOG(LogTemp, Warning, TEXT("Entering DieState for %s"), *Me->GetName());

	if (Me)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDie() 호출"));
		Me->OnDie();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Me is null in DieState"));
	}

	// Tick Enabled
	SetComponentTickEnabled(false);
}


// 랜덤 위치 가져오기
bool USunnyEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;
}