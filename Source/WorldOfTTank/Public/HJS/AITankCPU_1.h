// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HJS/AITank_1.h"
#include "AITankCPU_1.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API AAITankCPU_1 : public AAITank_1
{
	GENERATED_BODY()
	
public:
	AAITankCPU_1();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
virtual void BeginPlay() override;

private:

	// 컴포넌트
	UPROPERTY()
	class AAITankPlayer_1* PlayerTank;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* PawnSensingComponent;
	// 변수
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRange = 1300.f;
	UPROPERTY(EditAnywhere, Category = "combat")
	float FireRate = 2.f;
	// Handle
	UPROPERTY()
	FTimerHandle FireRateTimerHandle;

	// 함수
	// AI가 이동할 방향을 결정하는 함수, 타이머로 일정 시간마다 호출해보자.
	UFUNCTION()
	void SetMoveVector();

	UFUNCTION()
	bool InFireRange();

	UFUNCTION()
	void CheckFirecCondition();

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);
};
