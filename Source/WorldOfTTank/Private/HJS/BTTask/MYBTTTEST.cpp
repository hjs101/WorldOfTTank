// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/BTTask/MYBTTTEST.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
UMYBTTTEST::UMYBTTTEST()
{
    NodeName = TEXT("Find Position To AttackS");
}
EBTNodeResult::Type UMYBTTTEST::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    // 유효한 공격 위치 찾기
    AttackPosition = MyTank->FindValidAttackPosition(Target);

    if (AttackPosition == MyTank->GetActorLocation()) {
        return EBTNodeResult::Failed;
    }

    // AI를 해당 위치로 이동
    OwnerComp.GetAIOwner()->MoveToLocation(AttackPosition);
    bIsMoving = true;
    //UE_LOG(LogTemp, Error, TEXT("1111"));
    return EBTNodeResult::InProgress;
}

void UMYBTTTEST::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UE_LOG(LogTemp, Error, TEXT("%f"), abs(FVector::Dist(MyTank->GetActorLocation(), AttackPosition)));
    if (bIsMoving)
    {
        if (abs(FVector::Dist(MyTank->GetActorLocation(), AttackPosition)) < 2.0)
        {
            bIsMoving = false;
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}