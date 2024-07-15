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
public:
	void VictorySetting(float ClearTime);

	void LoseSetting();

	void ResultUISetting(bool bVictory, TArray<FString> Ranking);

	void ResultUIToVisible();
protected:
	virtual void NativeConstruct() override;
private:
	
	UPROPERTY()
	FTimerHandle FadeInTimerHandle;



	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UMissionResultUI* LoseUI;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UMissionResultUI* VictoryUI;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class URankingWidget* RankingUI;

};
