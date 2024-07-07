// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/BTTask/BTTask_SearchForTargets.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "Sunny/SunnyEnemy.h"
#include "Perception/PawnSensingComponent.h"

UBTTask_SearchForTargets::UBTTask_SearchForTargets()
{
	NodeName = "Search For Targets";
}

EBTNodeResult::Type UBTTask_SearchForTargets::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetPlayer = BlackboardComp->GetValueAsObject(FName("TargetPlayer"));
	UObject* TargetCPU = BlackboardComp->GetValueAsObject(FName("TargetCPU"));

	if (TargetCPU && Cast<ASunnyEnemy>(TargetCPU) && Cast<ASunnyEnemy>(TargetCPU)->IsDie()) {
		BlackboardComp->SetValueAsObject(FName("TargetCPU"), nullptr);

		if (Cast<ASunnyEnemy>(BlackboardComp->GetValueAsObject(FName("MainTarget"))))
		{
			BlackboardComp->SetValueAsObject(FName("MainTarget"), nullptr);
		}
	}

	if (TargetPlayer || TargetCPU)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}