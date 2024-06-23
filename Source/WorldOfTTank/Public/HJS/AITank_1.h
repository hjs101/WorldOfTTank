// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AITank_1.generated.h"

UCLASS()
class WORLDOFTTANK_API AAITank_1 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAITank_1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BodyMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* WhellMesh_1;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* WhellMesh_2;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* HeadMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BarrelMesh;
	UPROPERTY(VisibleAnywhere, Category = "Conponents", BlueprintReadOnly, meta = (AllowPrivateAccess = true));
	USceneComponent* ProjecttileSpawnPoint;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* CameraComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UFloatingPawnMovement* MovementComponent;

	// 변수
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 500;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 125;
	int moveState = 0;

	// class 변수
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AAIProjecttile_1> ProjecttileClass;

	// 함수
	UFUNCTION()
	void BodyTurn(float value);
	UFUNCTION()
	void Move(float value);
	UFUNCTION()
	void Fire();
};
