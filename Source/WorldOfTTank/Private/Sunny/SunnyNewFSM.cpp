// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyNewFSM.h"
#include "Sunny/SunnyNewTTank.h"
#include "Sunny/SunnyAIController.h"

#include "CSW/PlayerTankVehicle.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"


// Sets default values for this component's properties
USunnyNewFSM::USunnyNewFSM()
{
	// 초기화 시 Null 포인터 접근을 방지하기 위해 Owner 확인
	if (GetOwner() == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("USunnyEnemyFSM constructor called with null owner!"));
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
	AiController = Cast<ASunnyAIController>(Me->GetController());
	
}


// Called every frame
void USunnyNewFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 메세지 출력하기
	FString logMsg = UEnum::GetValueAsString(SunnyAiState);
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	// 대상 옆에 글자 표시
	DrawDebugString(GetWorld(), Me->GetActorLocation(), logMsg, 0, FColor::Yellow, 0, true);

	switch (SunnyAiState)
	{
	case ESunnyAiState::Idle:
		IdleState();
		break;
	case ESunnyAiState::Find:
		FindState();
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



void USunnyNewFSM::IdleState()
{
	//UE_LOG(LogTemp, Warning, TEXT("IdleState()"));

	//1. 시간이 흘렀으니까
	CurrentTime += GetWorld()->DeltaTimeSeconds;

	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (CurrentTime > IdleDelayTime)
	{
		// 3. 이동 상태로 전환하고 싶다
		SunnyAiState = ESunnyAiState::Find;
		//UE_LOG(LogTemp, Warning, TEXT("Change IdleState->MoveState"));
		// 경과 시간 초기화
		CurrentTime = 0;

		// AiController와 Me가 유효한지 확인
		if (AiController && Me)
		{
			// 최초 램덤한 위치 정해주기
			if (!AiController->GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos))
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to get random position in nav mesh."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AiController or Me is nullptr."));
		}
	}
}

/*  ==============================================================================================================================  */
void USunnyNewFSM::FindState()
{
	if (this && Me->InFireRange())
	{
		//UE_LOG(LogTemp, Warning, TEXT("ASunnyNewTTank::Tick() ---> InFireRange()"))
		Me->RotateTurret(Target->GetActorLocation());
		//RotateTurret(PlayerTTank->GetActorLocation());
	}

	Me->SetBeamLocation();
}

void USunnyNewFSM::MoveState()
{
	//UE_LOG(LogTemp, Warning, TEXT("MoveState()"));
	FVector destination;
	float DeltaTime;

	// 타깃이 존재하는지 확인
	if (Target)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Target Found"));
		destination = Target->GetActorLocation();

		if (nullptr != Me)
		{
			//UE_LOG(LogTemp, Warning, TEXT("MoveState()  -->  Rotate Turret"));
			AiController = Cast<ASunnyAIController>(Me->GetController());
			AiController->FindTargetPath(destination);
			AiController->MoveAlongPath(DeltaTime);

			Me->RotateTank(destination);
		}
	}
	else
	{
		// 타깃이 없다면 Idle 상태로 전환
		SunnyAiState = ESunnyAiState::Idle;
		//UE_LOG(LogTemp, Warning, TEXT("Change MoveState->IdleState"));
		return;
	}

	// NavigationSystem 객체 얻어오기
	//auto ns = UNavigationSystemV1::GetCurrent(GetWorld());
	//if (!ns)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("NavigationSystem is null in USunnyEnemyFSM::MoveState"));
	//	return;
	//}

	//// 목적지 길 찾기 경로 데이터 검색
	//FPathFindingQuery query;
	//FAIMoveRequest req;
	//// 목적지에서 인지할 수 있는 범위
	//req.SetAcceptanceRadius(3);
	//req.SetGoalLocation(destination);
	//// 길 찾기를 위한 쿼리 생성
	//AiController->BuildPathfindingQuery(req, query);
	//// 길 찾기 결과 가져오기
	//FPathFindingResult r = ns->FindPathSync(query);
	//// 목적지까지의 길 찾기 성공 여부 확인
	//if (r.Result == ENavigationQueryResult::Success)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("ENavigationQueryResult::Success"));
	//	
	//	// 타깃쪽으로 이동 // 저번에 그래서...어떻게 해야해? 애니메이션 하는 거랑. 뭐.. 한다고 했는데. 안되서. ㅋㅋ
	//	// 몸돌리는 거 + 이동을 분리할 건데, 이동도 이미 있어요  에이아이컨트롤러에서 만들ㅇ이ㅓ야/하/ㄴ ㅡ그래?? ㅇ오옹

	//	AiController->MoveToLocation(destination);
	//	//UE_LOG(LogTemp, Warning, TEXT("AiController->MoveToLocation(destination)"));

	//	//if (nullptr != Me)
	//	//{
	//	//	UE_LOG(LogTemp, Warning, TEXT("Move Sunny Ai TTank"));
	//	//	Me->Move(1);
	//	//	UE_LOG(LogTemp, Warning, TEXT("Move Forward"));
	//	//}
	//}
	//else
	//{
	//	// 보다시피  이제 무브투로케이션으로 못움직이죠 응으 ㅇ 몸체 트는 것도 안 되는 건가?? 몸 돌리는 것도 안되네요 응응 그거 목 돌리듯이 바꿔볼게.
	//	// 몸 돌리는 건   목 돌리는 거랑 달라서  setyawinput> 저부분 참고해ㅑ 하구여
	//	// 플레이어 방향으로 회전 -> 회전 다 끝났으면 직진 -> 이런방식으로 해야 하는데
	//	// nav mesh 시스템하고도 연계해야 해요
	//	// 힣잉.,... 어려운가?? 
	//	// 좀 어렵긴 해요 어디 부분 참고해서 볼까?? 
	//	// 제 코드중에 에이아이 컨트롤러 부분 있는데, 누나는 지금 에이아이 컨트롤러 없으니까 기존 에이아이 컨트롤러 상속받아서 만들어야하고
	//	// 적어드릴게여
	//	// 1. 에이아이 컨트롤러 만들기
	//	// 2. nav path 경로대로 회전 -> 이동의 무브 함수 만들기 -> AITankController 참고
	//	// 3. nav path 를 탐색해서 ai controller 에 path 설정 해주기 FindPositionToAttack 참고
	//	// 일단 시도만 해보고 안될 거 같으면 베타로 넘겨요 :) ㅋㅋ 나 다 못 할거 같은 느낌... ㅋㅋㅋ 우선... 체력바 안 깍이는 거 먼저 좀 봐줘... 그래야 내일 영상이라도 찍지. ㅋㅋ
	//	// //랜덤 위치로 이동
	//	auto result = AiController->MoveToLocation(RandomPos);
	//	 //목적지에 도착하면
	//	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	//	{
	//		// 새로운 랜덤 위치 가져오기
	//		AiController->GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
	//	}
	//}

	//if (nullptr != Me)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Move Sunny Ai TTank"));
	//	Me->Move(1);
	//	UE_LOG(LogTemp, Warning, TEXT("Move Forward"));
	//}


	// //타깃과 가까워지면 공격 상태로 전환하고 싶다
	// //1. 만약 거리가 공격 범위 안에 들어오면
	//if (FVector::Dist(destination, Me->GetActorLocation()) <= AttackRange)
	//{
	//	// 길찾기 기능 정지
	//	AiController->StopMovement();

	//	// 2. 공격 상태로 전환하고 싶다
	//	SunnyAiState = ESunnyAiState::Attack;

	//	if (nullptr != Me)
	//	{
	//		//UE_LOG(LogTemp, Warning, TEXT("Attack Sunny Ai TTank"));
	//		Me->Move(0);
	//		//UE_LOG(LogTemp, Warning, TEXT("Stop Move"));
	//	}
	//}
}

void USunnyNewFSM::AttackState()
{
	//UE_LOG(LogTemp, Warning, TEXT("AttackState()"));

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
		AiController->GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos);
	}
}

void USunnyNewFSM::DieState()
{

	//UE_LOG(LogTemp, Warning, TEXT("DieState()"));
	//UE_LOG(LogTemp, Warning, TEXT("Entering DieState for %s"), *Me->GetName());

	if (Me)
	{
		//UE_LOG(LogTemp, Warning, TEXT("OnDie() 호출"));
		Me->Dead();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Me is null in DieState"));
	}

	// Tick Enabled
	SetComponentTickEnabled(false);
}