// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MYBTTTEST.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UMYBTTTEST : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UMYBTTTEST();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    FVector AttackPosition;
    bool bIsMoving;
    UPROPERTY()
    class AAITankCPU_1* MyTank;
};