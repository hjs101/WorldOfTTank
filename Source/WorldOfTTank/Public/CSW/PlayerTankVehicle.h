// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/TankVehicle.h"
#include "PlayerTankVehicle.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API APlayerTankVehicle : public ATankVehicle
{
	GENERATED_BODY()

public:
	APlayerTankVehicle();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	FVector GetCursorTarget() const;
	void	EndIntro();
	
private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	class USpringArmComponent* FpsSpringArmComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UCameraComponent* FpsCameraComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UCameraComponent* IntroCameraComp;

	UPROPERTY(VisibleAnywhere)
	APlayerController* ControllerRef;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> TpsAimClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> FpsAimClass;

	TStrongObjectPtr<UUserWidget> CurrentAim;

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* ChasingAim;
	
	UPROPERTY(EditAnywhere)
	class UPaperSpriteComponent* Indicator;

	UPROPERTY(EditAnywhere, Category="FX")
	USoundBase* OuterFireSound;

	UPROPERTY(EditAnywhere, Category="FX")
	USoundBase* InnerFireSound;
	
	UPROPERTY(EditAnywhere, Category="FX")
	USoundBase* TrackSound;
	

	bool IsFps = false;
	bool IsIntro = true;
	class AAITankCPU_1* EncounterEnemy;

	void	MoveIntroCamera();
	FTimerHandle Timer;
	
	void	LerpZoom(float DeltaSeconds);
	void	ZoomIn();
	void	ZoomOut();
	void	ChangeToTps();
	void	ChangeToFps();
	
	
	float	ViewRotationRate = 100;
	float	CamDist[6] = {0, 400, 800, 1200, 1600, 2000};
	int		CamIdx = 4;
	void	LookRightLeft(float Value);
	void	LookUpDown(float Value);
};
