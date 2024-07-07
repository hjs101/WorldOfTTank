// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/BTTask/BTTask_FindPositionToAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/AITankController_1.h"
#include "HJS/AITankCPU_1.h"
#include "NavigationSystem.h"

UBTTask_FindPositionToAttack::UBTTask_FindPositionToAttack()
{
    NodeName = TEXT("Find Position To Attack");
}
EBTNodeResult::Type UBTTask_FindPositionToAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
    FVector AttackPosition = FVector::ZeroVector;
    for (int32 i = 10; i <= 80; i++) {
        float SampleRadius = i * 100;
        AttackPosition = AITank->FindValidAttackPosition(SampleRadius,Target);
        // 유효한 공격 위치 찾기
        if (AttackPosition != AITank->GetActorLocation()) {
            break;
        }
    }
    
    if (FVector::Dist(AttackPosition, AITank->GetActorLocation()) < 100.f ) {
        return EBTNodeResult::Failed;
    }
    // AI를 해당 위치로 이동
    //DrawDebugSphere(GetWorld(), AttackPosition, 50, 12, FColor::Green, false, 5.0f);
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
            req.SetGoalLocation(AttackPosition);
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