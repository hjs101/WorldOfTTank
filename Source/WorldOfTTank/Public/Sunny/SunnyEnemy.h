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

	void HandleDestruction();

	bool IsDie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	class APlayerTank* TTank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRange = 1000.f;

	FTimerHandle FireRateTimerHandle;
	float FireRate = 2.f;
	void CheckFireCondition();
	bool InFireRange();
	bool bDie = false;
	//// 체력
	//int32 Health = 3;
	//int32 Damage = 1;

	//class ASunnyGameMode* TTankGameMode;
	

public:

	// 적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class USunnyEnemyFSM* Fsm;

	// 적 움직임 (플레이어 움직임에 따라 폰 반응 : 중력 고려 x)
	UPROPERTY(VisibleAnywhere, BlueprintreadOnly, Category = Movement)
	class UFloatingPawnMovement* EnemyMove;

	// 공격 타이머 설정
	void SetFireTimer();
	// 공격 타이머 해제
	void ClearFIreTimer();

	//  죽음 상태 호출
	void OnDie();



	// NiagaraSystem
	UPROPERTY(EditAnyWhere)
	class UNiagaraSystem* BeamNiagara;

	void SetBeamLocation();
	void DrawBeam(FVector Start, FVector End);

};
