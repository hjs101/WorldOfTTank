// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChasingAim.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UChasingAim : public UUserWidget
{
	GENERATED_BODY()

	
public:
	void ScaleUp();
	void SetIsHit(bool Value);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	UUserWidget* OuterCircle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	UUserWidget* OuterCircleBg;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess))
	UUserWidget* InnerCircle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float Scale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool IsHit;
};
