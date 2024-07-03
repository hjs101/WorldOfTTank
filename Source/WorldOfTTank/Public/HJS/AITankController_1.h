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
	void FinishMove();

	FVector CurrentTargetLocation;
	bool bHasTarget;
	FNavPathSharedPtr CurrentPath;
	int32 CurrentPathPointIndex = 0;
	float AcceptanceRadius = 100.f;

	float CurrentTime = 0.f;
	float EndTime = 5.f;

	UPROPERTY()
    UBTTaskNode* CurrentTask;
};
