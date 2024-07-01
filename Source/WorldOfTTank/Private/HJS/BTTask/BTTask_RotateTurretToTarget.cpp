// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/BTTask/BTTask_RotateTurretToTarget.h"

UBTTask_RotateTurretToTarget::UBTTask_RotateTurretToTarget()
{
	NodeName = TEXT("Rotate Turret To Target");
	bNotifyTick = true;
	TargetActor = nullptr;
	AITank = nullptr;
}

EBTNodeResult::Type UBTTask_RotateTurretToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	TargetActor = Cast<APawn>(BlackboardComp->GetValueAsObject(FName("MainTarget")));
	if (TargetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
	if (AITank == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	CurrentTime = 0;
	return EBTNodeResult::InProgress; // 회전 중
}

void UBTTask_RotateTurretToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (AITank == nullptr || TargetActor == nullptr)
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

	// 포신 회전 시작
	AITank->RotateTurret(TargetActor->GetActorLocation());
	// 포신 회전 완료 여부 확인
	if (AITank->IsTurretRotationComplete(TargetActor))
	{
		for (int32 i = 0; i < 10; i++) 
		{
			AITank->RotateTurret(TargetActor->GetActorLocation());
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 회전 완료
		return;
	}
}