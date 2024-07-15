// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RankingStruct.generated.h"
/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API URankingStruct : public UObject
{
	GENERATED_BODY()
};

USTRUCT(Atomic, BlueprintType)
struct WORLDOFTTANK_API FRankingData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking")
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranking")
	float PlayTime;
};