// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sunny/SunnyBasePawn.h"
#include "SunnyTTank.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API ASunnyTTank : public ASunnyBasePawn
{
	GENERATED_BODY()

public:
	ASunnyTTank();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();

	APlayerController* GetTankPlayerController() const { return TTankPlayerController; }

	//bool TTankAlive = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Camera spring arm + Cameara
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* Camera;

	// Variables Declaration for Tank Movement
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 400.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 45.f;

	// Function Declaration for Tank Movement 
	void Move(float Value);
	void Turn(float Value);

	APlayerController* TTankPlayerController;

	//class ASunnyEnemyFSM* obj;

	// 라인 트레이스 길이
	float LineDistance = 500.f;




	// Enemy 여부 판단
	//void CheckEnemy();
};
