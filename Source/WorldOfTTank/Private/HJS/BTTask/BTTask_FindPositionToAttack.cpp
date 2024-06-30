// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/BTTask/BTTask_FindPositionToAttack.h"

UBTTask_FindPositionToAttack::UBTTask_FindPositionToAttack()
{
    NodeName = TEXT("Find Position To Attack");
    bNotifyTick = true;
    AITank = nullptr;
    bIsMoving = false;
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

    AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
    if (AITank == nullptr)
    {
        return EBTNodeResult::Failed;
    }
  
    for (int32 i = 10; i <= 30; i++) {
        float SampleRadius = i * 100;
        AttackPosition = AITank->FindValidAttackPosition(SampleRadius,Target);
        // 유효한 공격 위치 찾기
        if (AttackPosition != AITank->GetActorLocation()) {
            break;
        }
    }
    
    if (AttackPosition == AITank->GetActorLocation()) {
        return EBTNodeResult::Failed;
    }
    CurrentTime = 0;
    // AI를 해당 위치로 이동
    DrawDebugSphere(GetWorld(), AttackPosition, 50, 12, FColor::Green, false, 5.0f);
    OwnerComp.GetAIOwner()->MoveToLocation(AttackPosition);
    bIsMoving = true;
    //
    return EBTNodeResult::InProgress;
}

void UBTTask_FindPositionToAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
    }

    AITank->RotateTank(AttackPosition);

    if (bIsMoving)
    {
        if (abs(FVector::Dist(AITank->GetActorLocation(), AttackPosition)) < 300.f)
        {
            bIsMoving = false;
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}