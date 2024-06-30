// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HideBehindObstacle.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UBTTask_HideBehindObstacle : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_HideBehindObstacle();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	FVector HideLocation;
    bool bIsMoving;
    UPROPERTY()
    class AAITankCPU_1* AITank;
    float CurrentTime = 0.f;
    float FailTime = 3.f;
};