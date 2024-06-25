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


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Components", meta=(AllowPrivateAccess="true"))
	class UCapsuleComponent* CapsuleComp;

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
	UPROPERTY(EditAnywhere, Category="Movement")
	float Speed = 300;
	
	UPROPERTY(EditAnywhere, Category="Movement")
	float TurnRate = 50;


	bool	MoveState = true;
	

protected:
	void	RotateTurret(float Value);
	void	RotateBarrel(float Value);
	void	Move(float Value);
	void	Turn(float Value);
};
