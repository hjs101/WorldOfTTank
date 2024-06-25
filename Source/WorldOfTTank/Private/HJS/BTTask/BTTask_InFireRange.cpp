// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/BTTask/BTTask_InFireRange.h"
#include "HJS/AITankCPU_1.h"
#include "AIController.h"

UBTTask_InFireRange::UBTTask_InFireRange()
{
	NodeName = TEXT("In Fire Range");
}

EBTNodeResult::Type UBTTask_InFireRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 블랙보드 불러오기
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

	AAITankCPU_1* Tank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
	if (Tank == nullptr) {
		return EBTNodeResult::Failed;
	}

	if (Tank->InFireRange(Target))
	{
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
