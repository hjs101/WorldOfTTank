// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AITankController_1.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API AAITankController_1 : public AAIController
{
	GENERATED_BODY()
public:
	AAITankController_1();
	void SetNavPath(FNavPathSharedPtr Path);
	UFUNCTION()
	void SetCurrentTask(UBTTaskNode* Task);

	void SetbNonStopMove(bool Value);
	void SetAttackMode(bool Value);
	UFUNCTION()
	void StopBTT();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBlackboardData* BlackboardData;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBlackboardComponent* BlackboardComponent;

	UFUNCTION()
	void MoveAlongPath(float DeltaTime);

	UFUNCTION()
	void FinishMove(bool bSuccessed);

	FVector CurrentTargetLocation;
	bool bHasTarget;
	FNavPathSharedPtr CurrentPath;
	int32 CurrentPathPointIndex = 0;
	const float AcceptanceRadius = 500.f;

	float CurrentTime = 0.f;
	const float EndTime = 10.f;
	const float TargetHeightOffset = 50.f;

	bool bNonStop = false;
	bool bAttackMode = false;

	float MoveState = 0.f;
	float TurnState = 0.f;
	UPROPERTY()
    UBTTaskNode* CurrentTask;
};
