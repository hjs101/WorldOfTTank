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
	// 함수
	UFUNCTION()
	void BodyTurn(float value);
	UFUNCTION()
	void Move(float value);
	UFUNCTION()
	void Fire();
	UFUNCTION()
	void RotateTurret(FVector LookAtTarget);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BodyMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* WheelMesh_1;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* WheelMesh_2;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* HeadMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BarrelMesh;
	UPROPERTY(VisibleAnywhere, Category = "Conponents", BlueprintReadOnly, meta = (AllowPrivateAccess = true));
	USceneComponent* ProjecttileSpawnPoint;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UFloatingPawnMovement* MovementComponent;

	// 변수
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 500;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 125;
	int MoveState = 0;

	// class 변수
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AAIProjecttile_1> ProjecttileClass;

	// 함수

};
