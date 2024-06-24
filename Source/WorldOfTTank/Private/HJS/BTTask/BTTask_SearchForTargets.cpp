// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "Perception/PawnSensingComponent.h"
#include "HJS/BTTask/BTTask_SearchForTargets.h"

UBTTask_SearchForTargets::UBTTask_SearchForTargets()
{
	NodeName = "Search For Targets";
}

EBTNodeResult::Type UBTTask_SearchForTargets::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAITankCPU_1* AIPawn = Cast<AAITankCPU_1>(AIController->GetPawn());
	if (AIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UPawnSensingComponent* PawnSensingComponent = AIPawn->FindComponentByClass<UPawnSensingComponent>();
	if (PawnSensingComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* SensedPawn = nullptr;

	// If the target actor is set in the blackboard, the task is successful
	if (OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor") != nullptr)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}