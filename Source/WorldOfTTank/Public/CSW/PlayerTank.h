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

private:
	void	LookRightLeft(float Value);
	void	LookUpDown(float Value);
	FVector	GetCursorTarget() const;

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
	APlayerController* ControllerRef;

	float	CamDist[6] = {-200, 100, 400, 700, 1000, 1300};
	int		CamIdx = 5;

	void LerpZoom(float DeltaSeconds);
	void LerpAim(float DeltaSeconds);

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* ChasingAim;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> TpsAimClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> FpsAimClass;

	TStrongObjectPtr<UUserWidget> CurrentAim;

	void	ChangeToTps();
	void	ChangeToFps();
	
};
