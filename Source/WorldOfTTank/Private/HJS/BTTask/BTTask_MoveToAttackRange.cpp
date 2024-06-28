// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/BTTask/BTTask_MoveToAttackRange.h"

UBTTask_MoveToAttackRange::UBTTask_MoveToAttackRange()
{
	NodeName = TEXT("Move To Attack Range");
	bNotifyTick = true;
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

    MyTank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
    if (MyTank == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    MovePosition = MyTank->FindValidAttackRange(Target);

    if (MovePosition == MyTank->GetActorLocation()) {
        return EBTNodeResult::Failed;
    }

    OwnerComp.GetAIOwner()->MoveToLocation(MovePosition);
    bIsMoving = true;
    CurrentTime = 0;
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToAttackRange::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

    MyTank->RotateTank(MovePosition);

    if (bIsMoving)
    {
        if (abs(FVector::Dist(MyTank->GetActorLocation(), MovePosition)) < 300.f)
        {
            bIsMoving = false;
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}
