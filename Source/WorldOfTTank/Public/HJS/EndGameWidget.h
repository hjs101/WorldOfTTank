// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()
	

private:
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UMissionResultUI* LoseUI;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UMissionResultUI* VictoryUI;

};
