// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/BTTask/BTTask_FireAtTarget.h"

UBTTask_FireAtTarget::UBTTask_FireAtTarget()
{
	NodeName = TEXT("Fire At Target");
}

EBTNodeResult::Type UBTTask_FireAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// Fire at the target
	MyTank->Fire();

	return EBTNodeResult::Succeeded;
}