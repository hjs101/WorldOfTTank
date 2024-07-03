// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/Tank.h"
#include "PlayerTank.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API APlayerTank : public ATank
{
	GENERATED_BODY()

public:
	APlayerTank();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void	ChangeToTps();
	
	UFUNCTION(BlueprintImplementableEvent)
	void	ChangeToFps();

private:
	void	LookRightLeft(float Value);
	void	LookUpDown(float Value);

	bool	MoveState = true;
	
	void	ZoomIn();
	void	ZoomOut();
	
private:
	UPROPERTY(VisibleAnywhere, Category="Componets")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UCameraComponent* CameraComp;
	
	UPROPERTY(EditAnywhere, Category="Look")
	float	ViewRotationRate = 100;

	UPROPERTY(VisibleAnywhere)
	AController* ControllerRef;

	float	CamDist[6] = {-200, 100, 400, 700, 1000, 1300};
	int		CamIdx = 5;

	void LerpZoom(float DeltaSeconds);
};
