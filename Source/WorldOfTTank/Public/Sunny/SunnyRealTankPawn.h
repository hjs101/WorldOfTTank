// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "SunnyRealTankPawn.generated.h"


UCLASS()
class WORLDOFTTANK_API ASunnyRealTankPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	ASunnyRealTankPawn();

	virtual void Tick(float DeltaTime) override;

	
	// 탱크 몸체 회전
	void RotateTank(FVector LookAtTarget);

	// 탱크 머리 회전
	void RotateTurret(FVector LookAtTarget);
	// 탱크 포 회전


	// 이동
	UFUNCTION()
	void Move(float value);

	// 발사
	void Fire();


	// 특정 폰 파괴
	void HandleDestruction();


	// 재질
	UPROPERTY(EditAnywhere, Category = "MatInstance")
	UMaterialInstanceDynamic* BodyMaterial;

	UPROPERTY(EditAnywhere, Category = "MatInstance")
	UMaterialInstanceDynamic* TracksMaterial;


	// 탱크 머리, 몸체가 타겟을 향할 때
	bool bFound = false;

	// 각속도 확인
	bool bStopTurn = false;

	UFUNCTION()
	void SetSpeed();


	// NiagaraSystem
	UPROPERTY(EditAnyWhere, Category = "Combat")
	class UNiagaraComponent* FireNiagara;

	// 발사 소리
	UPROPERTY(EditAnywhere, Category = "Combat")
	class USoundBase* FireSound;



protected:
	virtual void BeginPlay() override;

	// 미사일 발사 지점  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ProjectileSpawnPoint;

	

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASunnyProjectile> ProjectileClass;


	// 탱크 머리 각도
	float TurretAngle;

	// 이동 상태 표시
	float MoveState = 0.f;

	// 회전 변수
	//bool bStopTurn = false;
	float VehicleYaw = 0.f;


	 
};
