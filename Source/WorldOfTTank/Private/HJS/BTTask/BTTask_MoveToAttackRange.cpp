// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/BTTask/BTTask_MoveToAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/AITankController_1.h"
#include "HJS/AITankCPU_1.h"
#include "NavigationSystem.h"

UBTTask_MoveToAttackRange::UBTTask_MoveToAttackRange()
{
	NodeName = TEXT("Move To Attack Range");
}

EBTNodeResult::Type UBTTask_MoveToAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
    if (AITank == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    FVector MovePosition = AITank->FindValidAttackRange(Target);

    if (MovePosition == AITank->GetActorLocation()) {
        return EBTNodeResult::Failed;
    }

    AAITankController_1* AIController = Cast<AAITankController_1>(OwnerComp.GetAIOwner());
    if (AIController != nullptr)
    {
        AIController->SetCurrentTask(this);
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
        if (NavSys != nullptr)
        {
            FPathFindingQuery Query;
            FAIMoveRequest req;
            req.SetAcceptanceRadius(3);
            req.SetGoalLocation(MovePosition);
            AIController->BuildPathfindingQuery(req, Query);

            FPathFindingResult Result = NavSys->FindPathSync(Query);

            if (Result.IsSuccessful())
            {
                AIController->SetbNonStopMove(true);
                AIController->SetNavPath(Result.Path);
                return EBTNodeResult::InProgress;
            }
        }
    }
	return EBTNodeResult::Failed;
}