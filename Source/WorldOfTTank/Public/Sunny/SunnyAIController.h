// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SunnyAIController.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASunnyAIController();

	class ASunnyNewTTank* SunnyNewTTank;
	class APlayerTankVehicle* PlayerTTank;
	class USunnyNewFSM* FSM;


	void SetNavPath(FNavPathSharedPtr Path);


	UFUNCTION()
	void MoveAlongPath(float DeltaTime);

	UFUNCTION()
	void FinishMove(bool bSuccessed);

	UFUNCTION()
	void FindTargetPath(FVector destination);




	// 길 찾기 수행시 랜덤 위치
	FVector RandomPos;
	// 랜덤 위치 가져오기
	bool GetRandomPositionInNavMesh(FVector centerLocaion, float radius, FVector& dest);

	



	FNavPathSharedPtr CurrentPath;
	int32 CurrentPathPointIndex = 0;
	float AcceptanceRadius = 500.f;

	float CurrentTime = 0.f;
	float EndTime = 10.f;

	bool bNonStop = false;
	bool bAttackMode = false;

	float MoveState = 0.f;
	float TurnState = 0.f;



protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
};
