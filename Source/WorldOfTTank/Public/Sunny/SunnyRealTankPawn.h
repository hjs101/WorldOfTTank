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

	
	// 탱크 몸체 회전
	void RotateTank(FVector LookAtTarget);

	// 탱크 머리 회전
	void RotateTurret(FVector LookAtTarget);
	// 탱크 포 회전


	// 이동
	void Move(float value);

	// 발사
	void Fire();


	// 특정 폰 파괴
	void HandleDestruction();


	// 미사일 발사 지점  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ProjectileSpawnPoint;


	// 재질
	UPROPERTY(EditAnywhere, Category = "MatInstance")
	UMaterialInstanceDynamic* BodyMaterial;

	UPROPERTY(EditAnywhere, Category = "MatInstance")
	UMaterialInstanceDynamic* TracksMaterial;


	// 탱크 머리, 몸체가 타겟을 향할 때
	bool isFound = false;


protected:
	virtual void BeginPlay() override;

	



private:

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASunnyProjectile> ProjectileClass;


	// 탱크 머리 각도
	float TurretAngle;

	


	 
};
