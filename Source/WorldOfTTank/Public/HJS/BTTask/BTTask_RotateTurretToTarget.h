// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateTurretToTarget.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UBTTask_RotateTurretToTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_RotateTurretToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	AActor* TargetActor;
	class AAITankCPU_1* MyTank;

	bool bIsRotating;

};