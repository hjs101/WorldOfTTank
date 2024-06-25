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
	MyTank = nullptr;
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

	MyTank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
	if (MyTank == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress; // 회전 중
}

void UBTTask_RotateTurretToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (MyTank == nullptr || TargetActor == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	// 포신 회전 시작
	MyTank->RotateTurret(TargetActor->GetActorLocation());
	// 포신 회전 완료 여부 확인
	if (MyTank->IsTurretRotationComplete(TargetActor))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 회전 완료
	}
}