// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPositionToAttack.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UBTTask_FindPositionToAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTTask_FindPositionToAttack();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    FVector AttackPosition;
    bool bIsMoving;
    UPROPERTY()
    class AAITankCPU_1* MyTank;
    float CurrentTime = 0.f;
    float FailTime = 3.f;
};