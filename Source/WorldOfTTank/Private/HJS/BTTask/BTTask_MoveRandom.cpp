// Fill out your copyright notice in the Description page of Project Settings.
#include "HJS/BTTask/BTTask_MoveRandom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "HJS/AITankCPU_1.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveRandom::UBTTask_MoveRandom()
{
    NodeName = "Move Random";
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveRandom::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());

    if (AITank == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys == nullptr)
    {
        return EBTNodeResult::Failed;
    }
    float SampleRadius = 1000.0;
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

    int32 RandomNumber = FMath::RandRange(0, SuccessPoints.Num()-1);
    MoveLocation = SuccessPoints[RandomNumber];
    OwnerComp.GetAIOwner()->MoveToLocation(MoveLocation);
    CurrentTime = 0;
    bIsMoving = true;
    return EBTNodeResult::InProgress;
}

void UBTTask_MoveRandom::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (AITank == nullptr)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    CurrentTime += DeltaSeconds;

    if (CurrentTime >= FailTime) {
        CurrentTime = 0;
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AITank->RotateTank(MoveLocation);

    if (bIsMoving)
    {
        if (abs(FVector::Dist(AITank->GetActorLocation(), MoveLocation)) < 250.f)
        {
            bIsMoving = false;
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }
    return;
}
