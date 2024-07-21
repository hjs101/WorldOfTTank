// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TotalState.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UTotalState : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateTotalStateUI(int32 UpdateEnemy);

private:
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* EnemyCount;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Enemy_1;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Enemy_2;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Enemy_3;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Enemy_4;

};
