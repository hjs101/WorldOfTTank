// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RankingWidget.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API URankingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetRanking(TArray<FString> FileLines);
	UFUNCTION()
	void OnMyRestart();
	UFUNCTION()
	void OnMyQuit();
	UFUNCTION()
	void OnAddRanking();
	UFUNCTION()
	void SetResultTime(float EndTime);
	UFUNCTION()
	void SetDisableAddButton();

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UVerticalBox* RankingBox;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UEditableTextBox* Name;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UTextBlock* Time;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UButton* AddRanking;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UButton* Restart;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UButton* Quit;

	float ResultTime = 0.f;

};
