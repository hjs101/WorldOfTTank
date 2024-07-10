// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/AITankController_1.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "HJS/AITankCPU_1.h"
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

void AAITankController_1::SetbNonStopMove(bool Value)
{
	bNonStop = Value;
}

void AAITankController_1::SetAttackMode(bool Value)
{
	bAttackMode = Value;
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
	AAITankCPU_1* AITank = Cast<AAITankCPU_1>(GetPawn());
	if (AITank)
	{
		if(TurnState != 0)
		{
			MoveState = 0;
		};
		AITank->Move(MoveState);
		AITank->BodyTurn(TurnState);
	}
}

void AAITankController_1::MoveAlongPath(float DeltaTime)
{

	if(CurrentPath)
	{
		TArray<FNavPathPoint> PathPoints = CurrentPath->GetPathPoints();
		if (PathPoints.Num() <= 0) {
			FinishMove(false);
		}
		if (PathPoints.Num() > 1)
		{
			FVector CurrentLocation = GetPawn()->GetActorLocation();
			FVector TargetLocation = FVector::ZeroVector;
			if (CurrentPathPointIndex < PathPoints.Num())
			{
				TargetLocation = PathPoints[CurrentPathPointIndex];
			}
			FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
			AAITankCPU_1* AITank = Cast<AAITankCPU_1>(GetPawn());
			if(AITank)
			{
				MoveState=1;
				TurnState = AITank->RotateTank(TargetLocation);
				if(AITank->GetFireState() && !bNonStop)
				{
					FinishMove(true);
				}
				if (AITank->GetFireState() && bAttackMode == true)
				{
					AActor* Target = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("MainTarget")));
					if (Target != nullptr)
					{
						bool bNotAbleAttack = AITank->HasLineOfSightToTarget(CurrentLocation, Target);
						if (!bNotAbleAttack)
						{
							FinishMove(true);
						}
					}
				}
			}
			
			if (FVector::Dist(CurrentLocation, TargetLocation) < AcceptanceRadius)
			{
				CurrentPathPointIndex++;
				if (CurrentPathPointIndex >= PathPoints.Num())
				{
					// 경로 완료
					FinishMove(true);
				}
			}
		}
		CurrentTime += DeltaTime;
		if (CurrentTime > EndTime) {
			FinishMove(false);
		}
	}
	else {
		CurrentPathPointIndex = 0;
	}
}

void AAITankController_1::FinishMove(bool bSuccessed)
{
	CurrentPath = nullptr;
	CurrentPathPointIndex = 0;
	CurrentTime = 0;
	MoveState = 0;
	// BTT의 InProgress 상태를 Success로 변경
	UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComp)
	{
		if (bSuccessed) {
			BTComp->OnTaskFinished(CurrentTask, EBTNodeResult::Succeeded);
		}
		else {
			BTComp->OnTaskFinished(CurrentTask, EBTNodeResult::Failed);
		}
	}

	bNonStop = false;
	bAttackMode = false;
}

void AAITankController_1::StopBTT()
{
	FinishMove(true);
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(GetComponentByClass(UBehaviorTreeComponent::StaticClass()));
	if (BehaviorComp)
	{
		BehaviorComp->StopTree(EBTStopMode::Safe);  // 안전하게 Behavior Tree 중지

		BehaviorComp->DestroyComponent();
	}
}