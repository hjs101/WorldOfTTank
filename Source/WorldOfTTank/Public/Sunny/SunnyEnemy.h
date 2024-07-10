// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyBasePawn.h"
#include "SunnyEnemy.generated.h"

/*class UWidgetComponent;
class UProgressBar;
class UTextBlock;
class APlayerTank;
class USunnyEnemyFSM;
class UFloatingPawnMovement;
class UNiagaraSystem;*/


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


	// 체력바 Widget
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthWidgetComp;

	
private:
	class APlayerTank* TTank;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float FireRange = 1000.f;

	FTimerHandle FireRateTimerHandle;
	float FireRate = 2.f;
	void CheckFireCondition();
	bool InFireRange();


	
	

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

	// Enemy 몸체 회전
	void RotateTank(FVector LookAtTarget);



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
