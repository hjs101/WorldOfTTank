// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyBasePawn.h"
#include "SunnyEnemy.generated.h"


/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyEnemy : public ASunnyBasePawn
{
	GENERATED_BODY()

public:
	ASunnyEnemy();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	class ASunnyTTank* TTank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRange = 300.f;

public:

	// 적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class USunnyEnemyFSM* Fsm;

	// 적 움직임 (플레이어 움직임에 따라 폰 반응 : 중력 고려 x)
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = Movement)
	class UFloatingPawnMovement* EnemyMove;
};
