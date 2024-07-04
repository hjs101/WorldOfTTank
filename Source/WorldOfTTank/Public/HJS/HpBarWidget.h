// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UHpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

    UFUNCTION(BlueprintCallable, Category="Health")
    void UpdateHpBar(float HealthPercent);

    UFUNCTION(BlueprintCallable, Category="Health")
    void UpdateHpText(const FText& HealthText);


private:
    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UTextBlock* HPText;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UProgressBar* HPProgressBar;
};
