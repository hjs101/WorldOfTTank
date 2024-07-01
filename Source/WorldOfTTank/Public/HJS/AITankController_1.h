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

	//FVector CurrentTargetLocation;
	//bool bHasTarget;
	//FNavPathSharedPtr CurrentPath;
	// 배열
};
