// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionResultUI.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UMissionResultUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void PlayAnim();

protected:
	virtual void NativeConstruct() override;

private:
    UPROPERTY(Transient, Transient, meta = (BindWidgetAnim), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UWidgetAnimation* FadeIn;

};
