// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "TankVehicle.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ATankVehicle : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ATankVehicle();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

		
	void SetPlayerTankDamage(float Damage);

	void Move(float Value);
	void Turn(float Value);
	void Fire();
	void Brake();
	void	RotateTurret(float Value);
	void	RotateBarrel(float Value);
	USceneComponent* GetProjectileSpawnPoint() const;
	void GetCurrentHitPoint(FHitResult& Hit) const;
	void UpdateMovementSound();
	
private:
	float MoveState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float	ReloadTime = 3.62;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float	CurrentReloadTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float Hp = 700;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float MaxHp = 700;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category="Components")
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UParticleSystemComponent* GunFire;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UAudioComponent* FireSoundComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UAudioComponent* TrackSoundComp;

	UPROPERTY(EditAnywhere, Category="FX")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category="FX")
	USoundBase* TrackSound;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTankVehicleAnimInstance> AnimInstanceClass;

public:
	UAudioComponent* GetFireSoundComp() const;
	UAudioComponent* GetTrackSoundComp() const;
};
