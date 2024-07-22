// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUI.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UPauseUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetRanking(TArray<FString> FileLines);
	UFUNCTION()
	void OnMyRestart();
	UFUNCTION()
	void OnMyQuit();

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UVerticalBox* RankingBox;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UButton* Restart;

	UPROPERTY(EditAnywhere,meta=(BindWidget))
	class UButton* Quit;
};
