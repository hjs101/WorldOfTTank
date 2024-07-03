// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUserWidjet.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UPlayerUserWidjet : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

	UPROPERTY(meta= (BindWidget))
	class UImage* Crosshair;
};
