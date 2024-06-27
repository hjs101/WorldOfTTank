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
	
	// 사정거리 확인 함수
	UFUNCTION()
	bool InFireRange(APawn* OtherActor);
	
	// 공격가능 상태 확인 함수
	UFUNCTION()
	bool GetFireState();
	// 특정 액터 방향으로 라인트레이싱 사용 함수
	UFUNCTION()
	bool HasLineOfSightToTarget(AActor* TargetActor) const;
	
	// 터렛 회전 종료 확인 함수
	UFUNCTION()
	bool IsTurretRotationComplete(AActor* TargetActor)const;
	// 발포 함수 오버라이드
	virtual void Fire() override;
	// 공격 유효 지점을 찾는 함수
	UFUNCTION()
	FVector FindValidAttackPosition(const AActor* TargetLocation);
	UFUNCTION()
	FVector FindValidAttackRange(const AActor* TargetActor);
protected:
virtual void BeginPlay() override;

private:

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* PawnSensingComponent;
	UPROPERTY(VisibleAnywhere)
	class UNavigationSystemV1* NavSys;
	// 변수
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRange = 1300.f;
	UPROPERTY(EditAnywhere, Category = "combat")
	float DetectRate = 1.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DetectRange = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireRate = 4.f;
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bReadyFire = true;
	// Handle
	UPROPERTY()
	FTimerHandle DetectRateTimerHandle;
	UPROPERTY()
	FTimerHandle FireRateTimerHandle;

	class AAITankController_1* ai;

	// 함수
	UFUNCTION()
	void CheckDistance();
	UFUNCTION()
	void OnSeePawn(APawn* Pawn);
	UFUNCTION()
	void FireReady();

};
