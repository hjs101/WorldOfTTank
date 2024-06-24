// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"
#include "HJS/AITankPlayer_1.h"
#include "HJS/AITankCPU_1.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/AITankController_1.h"

AAITankCPU_1::AAITankCPU_1()
{
	// AI컨트롤러 지정
	AIControllerClass = AAITankController_1::StaticClass();
	// Pawn이 생성될 때 자동으로 AIController 할당하도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Create and initialize the Pawn Sensing Component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

}

void AAITankCPU_1::BeginPlay()
{
	Super::BeginPlay();

	PlayerTank = Cast<AAITankPlayer_1>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AAITankCPU_1::CheckFirecCondition, FireRate, true);
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAITankCPU_1::OnSeePawn);
}

// Called every frame
void AAITankCPU_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetMoveVector();
}

void AAITankCPU_1::SetMoveVector()
{

	// 1. 플레이어의 위치를 받아오기
	if (PlayerTank) {
		// 2. 플레이어가 사정거리에 있다면, 이동은 하지 않고 플레이어와 일직선 방향으로 회전
		if (InFireRange()) {
			RotateTurret(PlayerTank->GetActorLocation());
		}
		else {
			// 3. 플레이어가 사정거리 바깥에 있다면, 플레이어 방향으로 회전하며 이동
			// 3-1 플레이어 위치 방향 구하기
			FVector DestinationVector = PlayerTank->GetActorLocation() - GetActorLocation();
			DestinationVector.Normalize();
			
			Move(1);

			// 현재 위치에서 플레이어 위치까지의 벡터와 현재 전방 벡터를 사용하여 회전 방향 결정
			FVector ForwardVector = GetActorForwardVector();
			float DotProduct = FVector::DotProduct(ForwardVector, DestinationVector);
			FVector CrossProduct = FVector::CrossProduct(ForwardVector, DestinationVector);

			// 오른쪽으로 회전해야 하면 1, 왼쪽으로 회전해야 하면 -1
			float Value = (CrossProduct.Z > 0) ? 1 : -1;
			if (CrossProduct.Z == 0) {
				Value = 0;
			}
			BodyTurn(Value);
		}
	}



}

void AAITankCPU_1::CheckFirecCondition()
{
	if (PlayerTank == nullptr)
	{
		return;
	}

	if (InFireRange() && PlayerTank->bAlive)
	{
		Fire();
	}
}

bool AAITankCPU_1::InFireRange()
{
	// 탱크까지의 거리 구하기
	if (PlayerTank)
	{
		// 탱크가 사정거리에 있는지 확인
		float Distance = FVector::Dist(GetActorLocation(), PlayerTank->GetActorLocation());
		// 사정거리에 있다면 True 리턴
		if (Distance <= FireRange)
		{
			return true;
		}
	}
	return false;
}

void AAITankCPU_1::OnSeePawn(APawn* Pawn)
{
	AAITankController_1* AIController = Cast<AAITankController_1>(GetController());

	if (AIController == nullptr)
	{
		return;
	}
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboard();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	// Set the target in the blackboard
	BlackboardComp->SetValueAsObject(FName("TargetActor"), Pawn);
}