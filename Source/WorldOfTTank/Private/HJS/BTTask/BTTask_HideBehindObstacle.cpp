// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/BTTask/BTTask_HideBehindObstacle.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/AITankController_1.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/Obstacle.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"

UBTTask_HideBehindObstacle::UBTTask_HideBehindObstacle()
{
	NodeName = "Hide Behind Obstacle";
}

EBTNodeResult::Type UBTTask_HideBehindObstacle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());

    if (AITank == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AActor* ObstacleActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("DetectedObstacle")));
    if (ObstacleActor == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AObstacle* Obstacle = Cast<AObstacle>(ObstacleActor);
    if (Obstacle == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AWheeledVehiclePawn* MainTarget = Cast<AWheeledVehiclePawn>(BlackboardComp->GetValueAsObject(FName("MainTarget")));
    if (MainTarget == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    FVector HideLocation = FVector::ZeroVector;
    bool bFoundHideLocation = false;
    TArray<FVector> HiddenArr = Obstacle->GetLocationArr();

    for (const FVector& SamplePoint : HiddenArr)
    {

        if (SamplePoint == Obstacle->GetActorLocation()) {
            continue;
        }

        FVector EndLocation = MainTarget->GetMesh()->GetSocketLocation(FName("turret_jnt"));

        


        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(AITank);
        CollisionParams.AddIgnoredActor(MainTarget);
        float SphereRadius = 30.f;

        bool bHit = GetWorld()->SweepSingleByChannel(
            HitResult,
            SamplePoint,
            EndLocation,
            FQuat::Identity,
            ECC_Visibility, // 가시성 채널 사용 
            FCollisionShape::MakeSphere(SphereRadius),
            CollisionParams
        );

        if (!bHit)
        {
            continue;  // 타겟이 보이면 다음 위치로
        }
        else
        {
            if(AITank->CheckForNavSystem(SamplePoint)){
				HideLocation = SamplePoint;
				bFoundHideLocation = true;
				break;
            }
        }
    }
    if (bFoundHideLocation)
    {
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
                req.SetGoalLocation(HideLocation);
                AIController->BuildPathfindingQuery(req, Query);

                FPathFindingResult Result = NavSys->FindPathSync(Query);

                if (Result.IsSuccessful())
                {
                    AIController->SetNavPath(Result.Path);
                    return EBTNodeResult::InProgress;
                }
            }
        }
    }

    return EBTNodeResult::Failed;
}