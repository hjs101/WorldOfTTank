// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/BTTask/BTTask_CheckIfNoObstacle.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"

UBTTask_CheckIfNoObstacle::UBTTask_CheckIfNoObstacle()
{
	NodeName = TEXT("Check If No Obstacle");
}

EBTNodeResult::Type UBTTask_CheckIfNoObstacle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Target = Cast<APawn>(BlackboardComp->GetValueAsObject(FName("MainTarget")));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAITankCPU_1* MyTank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
	if (MyTank == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// 장애물이 없는지 확인
	if (MyTank->HasLineOfSightToTarget(MyTank->GetHeadLocation() ,Target))
	{
		return EBTNodeResult::Failed; // 장애물이 있는 경우 실패
	}

	return EBTNodeResult::Succeeded; // 장애물이 없는 경우 성공
}