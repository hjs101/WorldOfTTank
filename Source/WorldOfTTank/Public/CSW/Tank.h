// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

UCLASS()
class WORLDOFTTANK_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* BarrelMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* LeftWheelMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* RightWheelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	class UPhysicsConstraintComponent* LeftWheelSuspension;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	class UPhysicsConstraintComponent* RightWheelSuspension;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, Category="Movement")
	float Speed = 150;
	const float MaxSpeed = 400;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float TurnRate = 70;

	int MoveState = 0;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float	ReloadTime = 3.62;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float	CurrentReloadTime = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="State")
	float	HP = 700;

	void SetPlayerTankDamage(float Damage);

protected:
	void	RotateTurret(float Value);
	void	RotateBarrel(FVector Target);
	void	Move(float Value);
	void	Turn(float Value);
	void	Fire();
	void	Brake();
	FVector	GetCurrentHitPoint() const;
};
