// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "AITank_1.generated.h"

UCLASS()
class WORLDOFTTANK_API AAITank_1 : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAITank_1();
	UFUNCTION()
	void RotateTurret(FVector LookAtTarget);
	void RotateBarrel(FVector LookAtTarget);
	float RotateTank(FVector LookAtTarget);
	UFUNCTION()
	void BodyTurn(float value);
	UFUNCTION()
	void Move(float value);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 함수

	UFUNCTION()
	virtual void Fire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="MatInstance")
	UMaterialInstanceDynamic* BodyMaterial;

	UPROPERTY(EditAnywhere, Category = "MatInstance")
	UMaterialInstanceDynamic* TracksMaterial;

	UFUNCTION()
	void SetSpeed();

	UFUNCTION()
	void SetHead();

	UFUNCTION()
	void SetGun();

	UFUNCTION()
	void OnOutLine();

	UFUNCTION()
	void OffOutLine();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Conponents", BlueprintReadOnly, meta = (AllowPrivateAccess = true));
	USceneComponent* ProjecttileSpawnPoint;
	// 한계 고도 변수
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DownLimit = -3.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float UpLimit = 30.f;
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float LookPitch = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "SFX")
    UAudioComponent* TrackSoundComp;
private:	
	
	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//USkeletalMeshComponent* OutlineMesh;

    // Sound cue for the engine sound
    UPROPERTY(EditAnywhere, Category = "SFX")
    class USoundCue* TrackSoundCue;

	// 변수
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 125.f;
	int MoveState = 0;

	// 탄 쏘는 속도 : m/s 단위이기 때문에 ProjectTile Speed를 100으로 나눔
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireSpeed = 80.f;

	// 회전 변수
	bool bStopTurn = false;
	float VehicleYaw = 0.f;

	// class 변수
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AAIProjecttile_1> ProjecttileClass;

	// 함수
	UFUNCTION()
	float CalculateLaunchAngle(float LaunchSpeed, float TargetDistance, float TargetHeight);

	UPROPERTY(EditAnywhere, Category="Combat")
	UParticleSystem* GunFire;

	float TurretAngle;
	float GunElevation;
	UPROPERTY(EditAnywhere,Category="SFX")
	class UAudioComponent* FireSoundComp;
	UPROPERTY(EditAnywhere,Category="SFX")
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere)
	class UPaperSpriteComponent* Indicator;

	UPROPERTY()
	FTimerHandle OutlineTimerHandle;

private:
	void UpdateMovementSound();
};
