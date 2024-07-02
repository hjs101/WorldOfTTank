// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyEnemyFSM.h"
#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyTTank.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
USunnyEnemyFSM::USunnyEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USunnyEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 월드에서 ASunnyTTank 타깃 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ASunnyTTank::StaticClass());
	// ASunnyTTank 타입으로 캐스팅
	Target = Cast<ASunnyTTank>(actor);
	// 소유 객체 가져오기
	Me = Cast<ASunnyEnemy>(GetOwner());
	
}


// Called every frame
void USunnyEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 실행창에 상태 메세지 출력하기
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);


	switch (mState)
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
	case EEnemyState::Damage:
		DamageState();
		break;
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
		mState = EEnemyState::Move;
		// 경과 시간 초기화
		CurrentTime = 0;
	}
}

// 이동 상태
void USunnyEnemyFSM::MoveState() 
{
	// 1. 타깃 목적지가 필요하다
	FVector destination = Target->GetActorLocation();
	// 2. 방향이 필요하다
	FVector dir = destination - Me->GetActorLocation();
	// 3. 방향으로 이동하고 싶다
	Me->AddMovementInput(dir.GetSafeNormal());

	// 타깃과 가까워지면 공격 상태로 전환하고 싶다
	// 1. 만약 거리가 공격 범위 안에 들어오면
	if (dir.Size() < AttackRange)
	{
		// 2. 공격 사애로 전환하고 싶다
		mState = EEnemyState::Attack;
	}
}

// 공격 상태
void USunnyEnemyFSM::AttackState() 
{
	// 목표 : 일정 시간에 한 번씩 공격하고 싶다
	// 1. 시간이 흘러야 한다
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 공격 시간이 됐으니까
	if (CurrentTime > AttackDelayTime)
	{
		// 3. 공격하고 싶다
		UE_LOG(LogTemp, Warning, TEXT("Attack!!!!"));
		// 경과 시간 초기화
		CurrentTime = 0;
	}	

	// 목표 : 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다
	// 1. 타깃과의 거리가 필요하다
	float distance = FVector::Distance(Target->GetActorLocation(), Me->GetActorLocation());
	// 2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (distance > AttackRange)
	{
		// 3. 상태를 이동으로 전환하고 싶다
		mState = EEnemyState::Move;
	}
}

// 피격 상태
void USunnyEnemyFSM::DamageState() 
{

}

// 죽음 상태
void USunnyEnemyFSM::DieState() 
{

}