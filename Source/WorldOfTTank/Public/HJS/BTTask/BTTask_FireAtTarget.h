// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FireAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UBTTask_FireAtTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FireAtTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};