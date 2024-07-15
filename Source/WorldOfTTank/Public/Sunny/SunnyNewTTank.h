// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyRealTankPawn.h"
#include "SunnyNewTTank.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyNewTTank : public ASunnyRealTankPawn
{
	GENERATED_BODY()


public:
	ASunnyNewTTank();

	virtual void Tick(float DeltaTime) override;


	// 탱크 머리 회전 계산
	void CalcTurretRotation(AActor* TargetActor);


	// 이펙트, 사운드 삭제
	void HandleDestruction();


	bool IsDead();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	// 체력바 Widget
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthWidgetComp;


private:
	class APlayerTankVehicle* PlayerTTank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRange = 2000.f;

	FTimerHandle FireRateTimerHandle;
	float FireRate = 5.f;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class USunnyNewFSM* FSM;

	UPROPERTY(VisibleAnywhere,Category="Components")
	UStaticMeshComponent* OnDieMeshComp;

	// 공격 타이머 설정
	void SetFireTimer();
	// 공격 타이머 해제
	void ClearFIreTimer();

	//  죽음 상태 호출
	void Dead();


	// NiagaraSystem
	UPROPERTY(EditAnyWhere)
	class UNiagaraSystem* BeamNiagara;

	void SetBeamLocation();
	void DrawBeam(FVector Start, FVector End);



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

	bool bDead = false;


	// Enemy Indicator
	UPROPERTY(EditAnywhere)
	class UPaperSpriteComponent* EnemyIndicator;
	
};
