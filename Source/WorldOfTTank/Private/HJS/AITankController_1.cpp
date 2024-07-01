// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "HJS/AITankController_1.h"

AAITankController_1::AAITankController_1()
{
	// Initialize the BlackboardComponent
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
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

void AAITankController_1::Tick(float DeltaTime)
{
    
    //if (bHasTarget && CurrentPath.IsValid() && CurrentPath->GetPathPoints().Num() > 0)
    //{
    //    TArray<FNavPathPoint> PathPoints = CurrentPath->GetPathPoints();
    //    APawn* ControlledPawn = GetPawn();
    //    if (ControlledPawn)
    //    {
    //        FVector NextPathPoint = PathPoints[0];
    //        FVector Direction = (NextPathPoint - ControlledPawn->GetActorLocation()).GetSafeNormal();

    //        if (FVector::Dist(ControlledPawn->GetActorLocation(), NextPathPoint) < 100.f)
    //        {
    //            PathPoints.RemoveAt(0);
    //        }

    //        UFloatingPawnMovement* MovementComponent = Cast<UFloatingPawnMovement>(ControlledPawn->GetMovementComponent());
    //        if (MovementComponent)
    //        {
    //            MovementComponent->AddInputVector(Direction);
    //        }
    //    }
    //}
}
