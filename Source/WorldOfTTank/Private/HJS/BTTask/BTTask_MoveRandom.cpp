// Fill out your copyright notice in the Description page of Project Settings.
#include "HJS/BTTask/BTTask_MoveRandom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/AITankController_1.h"
#include "NavigationSystem.h"
#include "HJS/AITankCPU_1.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveRandom::UBTTask_MoveRandom()
{
    NodeName = "Move Random";
}

EBTNodeResult::Type UBTTask_MoveRandom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());

    if (AITank == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys == nullptr)
    {
        return EBTNodeResult::Failed;
    }   
    float SampleRadius = 1000.f;
    int32 NumSamples = 36; // 샘플링할 지점 수 (360도를 기준으로 10도 간격)
    FVector StartLocation = AITank->GetActorLocation();
    
    TArray<FVector> SuccessPoints;

    for (int32 i = 0; i < NumSamples; i++)
    {
        // 각도를 기준으로 샘플링 지점 계산
        float Angle = (360.0f / NumSamples) * i;
        FVector SamplePoint = StartLocation + SampleRadius * FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0);
        FPathFindingQuery query;
        FAIMoveRequest req;
        req.SetAcceptanceRadius(3);
        req.SetGoalLocation(SamplePoint);
        OwnerComp.GetAIOwner()->BuildPathfindingQuery(req, query);
        FPathFindingResult result = NavSys->FindPathSync(query);

        if (result.Result == ENavigationQueryResult::Success)
        {
            SuccessPoints.Push(SamplePoint);
        }
    }

    if (SuccessPoints.IsEmpty()) {
        return EBTNodeResult::Failed;
    }

    int32 RandomNumber = FMath::RandRange(0, SuccessPoints.Num()-1);
    FVector MoveLocation = SuccessPoints[RandomNumber];
    AAITankController_1* AIController = Cast<AAITankController_1>(OwnerComp.GetAIOwner());
    if (AIController != nullptr)
    {
        AIController->SetCurrentTask(this);
        FPathFindingQuery Query;
        FAIMoveRequest req;
        req.SetAcceptanceRadius(3);
        req.SetGoalLocation(MoveLocation);
        AIController->BuildPathfindingQuery(req, Query);

        FPathFindingResult Result = NavSys->FindPathSync(Query);

        if (Result.IsSuccessful())
        {
            AIController->SetbNonStopMove(true);
            AIController->SetAttackMode(true);
            AIController->SetNavPath(Result.Path);
            return EBTNodeResult::InProgress;
        }
    }

    return EBTNodeResult::Failed;
}