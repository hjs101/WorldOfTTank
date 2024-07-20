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
}


// Called every frame
void USunnyNewFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 메세지 출력하기
	FString logMsg = UEnum::GetValueAsString(SunnyAiState);
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	// 대상 옆에 글자 표시
	DrawDebugString(GetWorld(), Me->GetActorLocation(), logMsg, 0, FColor::Yellow, 0.f, true);

	//UE_LOG(LogTemp, Warning, TEXT("%d"), SunnyAiState);
	switch (SunnyAiState)
	{
	case ESunnyAiState::Idle:
		IdleState();
		//UE_LOG(LogTemp, Warning, TEXT("idle"));
		break;
	case ESunnyAiState::Find:
		FindState();
		//UE_LOG(LogTemp, Warning, TEXT("Find"));
		break;
	case ESunnyAiState::Move:
		MoveState();
		//UE_LOG(LogTemp, Warning, TEXT("Move"));
		break;
	case ESunnyAiState::Attack:
		AttackState();
		//UE_LOG(LogTemp, Warning, TEXT("Attack"));
		break;
	case ESunnyAiState::Die:
		DieState();
		UE_LOG(LogTemp, Warning, TEXT("Die"));
		break;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("default"));
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
		//SunnyAiState = ESunnyAiState::Move;
		//UE_LOG(LogTemp, Warning, TEXT("Change IdleState->MoveState"));
		// 경과 시간 초기화
		CurrentTime = 0;

		// AiController와 Me가 유효한지 확인
		if (AiController && Me)
		{
			// 최초 램덤한 위치 정해주기
			if (!AiController->GetRandomPositionInNavMesh(Me->GetActorLocation(), 500, RandomPos))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Failed to get random position in nav mesh."));
			}
		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("AiController or Me is nullptr."));
		}
	}
}

void USunnyNewFSM::FindState()
{
	//UE_LOG(LogTemp, Warning, TEXT("FindState()"));
	if (Target)
	{
		FVector direction = Target->GetActorLocation();
		//Me->RotateTurret(direction);

		if (Me->bTargetFound)
		{
			SunnyAiState = ESunnyAiState::Move;
			Me->bTargetFound = false;
		}


		// 플레이어가 탱크 정면에 있는지 확인
		//bool bIsPlayerInFront = IsPlayerInFront(direction, 10.f); // 10도 이내를 정면으로 간주
		//UE_LOG(LogTemp, Warning, TEXT("bIsPlayerInFront : %d"), bIsPlayerInFront);

		//if (bIsPlayerInFront)
		//{
		//	SunnyAiState = ESunnyAiState::Move;
		//}
		//else
		//{
		//	SunnyAiState = ESunnyAiState::Find;
		//}
		
	}
	
	
}

void USunnyNewFSM::MoveState()
{
	FVector destination;

	//  AI가 Path를 따라 이동 중일 때는 return
	if (bMoving)
	{
		return;
	}

	// 타깃이 존재하는지 확인
	if (Target)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Target Found"));
		destination = Target->GetActorLocation();

		if (nullptr != Me)
		{
			//UE_LOG(LogTemp, Warning, TEXT("MoveState()  --> Target==ture  --> nullptr != Me"));
			if (AiController)
			{
				AiController->FindTargetPath(destination);
				//UE_LOG(LogTemp, Warning, TEXT("after %d"), bMoving);
			}
		}
	}
	else
	{
		// 타깃이 없다면 Idle 상태로 전환
		SunnyAiState = ESunnyAiState::Idle;
		//UE_LOG(LogTemp, Warning, TEXT("Change MoveState->IdleState"));
		return;
	}
}

void USunnyNewFSM::AttackState()
{
	//AiController->FinishMove();
	//UE_LOG(LogTemp, Warning, TEXT("Succeed FinishMove()"));

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
	UE_LOG(LogTemp, Warning, TEXT("DieState()"));
	//UE_LOG(LogTemp, Warning, TEXT("Entering DieState for %s"), *Me->GetName());

	if (Me)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Dead() 호출"));
		Me->Dead();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Me is null in DieState"));
	}

	// Tick Enabled
	SetComponentTickEnabled(false);
}


//bool USunnyNewFSM::IsPlayerInFront(FVector PlayerLocation, float AngleThreshold)
//{
//	// 현재 메쉬의 앞쪽 벡터 정규화
//	FVector forwardVector = Me->GetActorForwardVector().GetSafeNormal();
//
//	// 플레이어 위치에서 현재 메쉬 위치를 뺀 벡터를 계산하여 direction에 저장
//	FVector direction = Target->GetActorLocation() - Me->GetActorLocation();
//	// 목표와의 높이 차이는 무시하므로 Z 값을 0으로 설정
//	direction.Z = 0.f;
//	// 목표 벡터를 정규화
//	direction.Normalize();
//
//	// ForwardVector와 Direction 간의 내적 계산
//	float forwardDot = FVector::DotProduct(forwardVector, direction);
//
//	// 각도 차이를 계산
//	//float angleDifference = FMath::RadiansToDegrees(acos(forwardDot));
//
//	// 디버깅 로그 추가
//	UE_LOG(LogTemp, Warning, TEXT("ForwardVector: %s, Direction: %s"), *forwardVector.ToString(), *direction.ToString());
//	UE_LOG(LogTemp, Warning, TEXT("ForwardDot: %f"), forwardDot);
//	//UE_LOG(LogTemp, Warning, TEXT("ForwardDot: %f, AngleDifference: %f"), forwardDot, angleDifference);
//
//	// 각도 차이가 임계값 이하인지 확인하여 플레이어가 정면에 있는지 여부를 반환
//	//return angleDifference <= AngleThreshold;
//	return forwardDot > 0.9f;
//}



void USunnyNewFSM::SettingbMoving(bool value)
{
	bMoving = value;
}

void USunnyNewFSM::SttingbAttacking(bool value)
{
	bAttacking = value;
}
