// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/AITankCPU_1.h"
#include "AIController.h"
#include "HJS/BTTask/BTTask_CheckAttackState.h"

UBTTask_CheckAttackState::UBTTask_CheckAttackState()
{
    NodeName = TEXT("Check Attack State");
}

EBTNodeResult::Type UBTTask_CheckAttackState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
    if (AITank == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    if (AITank->GetFireState())
    {
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}