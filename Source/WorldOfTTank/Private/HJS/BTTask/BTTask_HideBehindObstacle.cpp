// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/Obstacle.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HJS/BTTask/BTTask_HideBehindObstacle.h"

UBTTask_HideBehindObstacle::UBTTask_HideBehindObstacle()
{
	NodeName = "Hide Behind Obstacle";
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_HideBehindObstacle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    MyTank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());

    if (MyTank == nullptr)
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

    APawn* MainTarget = Cast<APawn>(BlackboardComp->GetValueAsObject(FName("TargetPlayer")));
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

        FVector EndLocation = MainTarget->GetActorLocation();

        FHitResult HitResult;
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(MyTank);
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
            if(MyTank->CheckForNavSystem(SamplePoint)){
				HideLocation = SamplePoint;
				bFoundHideLocation = true;
				break;
            }
        }
    }
    if (bFoundHideLocation)
    {
        CurrentTime = 0;
        bIsMoving = true;
        OwnerComp.GetAIOwner()->MoveToLocation(HideLocation);
        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

void UBTTask_HideBehindObstacle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (MyTank == nullptr)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    CurrentTime += DeltaSeconds;
    if (CurrentTime >= FailTime) {
        CurrentTime = 0;
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }

    MyTank->RotateTank(HidePosition);

    if (bIsMoving)
    {
        if (abs(FVector::Dist(MyTank->GetActorLocation(), HidePosition)) < 300.f)
        {
            bIsMoving = false;
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
