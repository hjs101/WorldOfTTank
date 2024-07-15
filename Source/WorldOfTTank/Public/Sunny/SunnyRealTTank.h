// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyRealTankPawn.h"
#include "SunnyRealTTank.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyRealTTank : public ASunnyRealTankPawn
{
	GENERATED_BODY()

public:
	ASunnyRealTTank();

	virtual void Tick(float DeltaTime) override;


	//  탱크 머리 회전
	void RotateTurret(FVector LookAtTarget);

	// 탱크 포 회전
	void RotateBarrel(FVector LookAtTarget);


	// 이펙트, 사운드 삭제
	void HandleDestruction();


	bool IsDie();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// 체력바 Widget
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthWidgetComp;


private:
	class APlayerTank* PlayerTTank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRange = 2000.f;

	FTimerHandle FireRateTimerHandle;
	float FireRate = 2.f;
	void CheckFireCondition();
	bool InFireRange();


	// 탱크 머리 각도
	float TurretAngle;

	// 탱크 포 각도
	float BerrelAngel;


	// 변수
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 125.f;
	int MoveState = 0;

	// 회전 변수
	bool bStopTurn = false;
	float VehicleYaw = 0.f;



public:

	// 적 AI 관리 컴포넌트 클래스
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	//class USunnyEnemyFSM* Fsm;

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

	// Enemy 몸체 회전
	//void RotateTank(FVector LookAtTarget);



	// 체력바 게이지
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentHealthLabel;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxHealthLabel;


	UPROPERTY(EditAnywhere)
	class USunnyHealth* HealthComp;

	UFUNCTION()
	//float GetHealthPercent(float health, float maxHealth);

	void SetHealthPercent(float health, float maxHealth);

	bool bDie = false;


	// Enemy Indicator
	UPROPERTY(EditAnywhere)
	class UPaperSpriteComponent* EnemyIndicator;

	
};
