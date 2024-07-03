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
#include "HJS/AITank_1.h"
AAITankController_1::AAITankController_1()
{
	// Initialize the BlackboardComponent
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AAITankController_1::SetNavPath(FNavPathSharedPtr Path)
{
	CurrentPath = Path;
	CurrentPathPointIndex = 0;
	CurrentTime = 0;
}

void AAITankController_1::SetCurrentTask(UBTTaskNode* Task)
{
	if (Task != nullptr) {
		CurrentTask = Task;
	}
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
	Super::Tick(DeltaTime);
	MoveAlongPath(DeltaTime);
}

void AAITankController_1::MoveAlongPath(float DeltaTime)
{
	if(CurrentPath)
	{
		TArray<FNavPathPoint> PathPoints = CurrentPath->GetPathPoints();
		if (PathPoints.Num() > 1)
		{
			FVector CurrentLocation = GetPawn()->GetActorLocation();
			FVector TargetLocation;
			if (CurrentPathPointIndex < PathPoints.Num())
			{
				TargetLocation = PathPoints[CurrentPathPointIndex];
			}
			FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
			GetPawn()->AddMovementInput(Direction);
			Cast<AAITank_1>(GetPawn())->RotateTank(TargetLocation);
			if (FVector::Dist(CurrentLocation, TargetLocation) < AcceptanceRadius)
			{
				CurrentPathPointIndex++;
				if (CurrentPathPointIndex >= PathPoints.Num())
				{
					// 경로 완료
					FinishMove();
				}
			}
		}
		CurrentTime += DeltaTime;
		if (CurrentTime > EndTime) {
			FinishMove();
		}
	}
	else {
		CurrentPathPointIndex = 0;
	}
}

void AAITankController_1::FinishMove()
{
	CurrentPath = nullptr;
	CurrentPathPointIndex = 0;
	CurrentTime = 0;
	// BTT의 InProgress 상태를 Success로 변경
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComp)
	{
		BTComp->OnTaskFinished(CurrentTask, EBTNodeResult::Succeeded);
	}
}

void AAITankController_1::StopBTT()
{
	FinishMove();
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(GetComponentByClass(UBehaviorTreeComponent::StaticClass()));
	if (BehaviorComp)
	{
		BehaviorComp->StopTree(EBTStopMode::Safe);  // 안전하게 Behavior Tree 중지

		BehaviorComp->DestroyComponent();
	}
}