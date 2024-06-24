// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "HJS/AITankController_1.h"

AAITankController_1::AAITankController_1()
{
	// Initialize the BlackboardComponent
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

}

UBlackboardComponent* AAITankController_1::GetBlackboard()
{
	return BlackboardComponent;
}

void AAITankController_1::BeginPlay()
{
	Super::BeginPlay();

	if (UseBlackboard(BlackboardData, BlackboardComponent))
	{
		if (BehaviorTree != nullptr)
		{
			RunBehaviorTree(BehaviorTree);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BehaviorTree is nullptr!"));
		}
	}
}
