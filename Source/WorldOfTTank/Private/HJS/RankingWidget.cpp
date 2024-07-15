// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/RankingWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Sunny/SunnyGameMode.h"
#include "Components/VerticalBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void URankingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Restart->OnClicked.AddDynamic(this, &URankingWidget::OnMyRestart);
	Quit->OnClicked.AddDynamic(this, &URankingWidget::OnMyQuit);
	AddRanking->OnClicked.AddDynamic(this, &URankingWidget::OnAddRanking);
}

void URankingWidget::OnMyRestart()
{
	if (ASunnyGameMode* gm = Cast<ASunnyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->RestartGame();
	}
}

void URankingWidget::OnMyQuit()
{
	if (ASunnyGameMode* gm = Cast<ASunnyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->QuitGame();
	}
}

// 랭킹에 추가될 때
void URankingWidget::OnAddRanking()
{
	if (ASunnyGameMode* gm = Cast<ASunnyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FString name = Name->GetText().ToString();

		if (name == "")
		{
			return;
		}

		gm->UpdateRanking(name, ResultTime);
		AddRanking->SetIsEnabled(false);
	}
}

void URankingWidget::SetResultTime(float EndTime)
{
	ResultTime = EndTime;
	int32 Minutes = FMath::FloorToInt(ResultTime / 60.0f);
	int32 Seconds = FMath::FloorToInt(ResultTime) % 60;
	int32 Milliseconds = FMath::RoundToInt((ResultTime - FMath::FloorToInt(ResultTime)) * 1000);
	FString ResultStr = FString::Printf(TEXT("%2d:%2d:%2d"), Minutes,Seconds,Milliseconds);
	if(Time)
	{
		Time->SetText(FText::FromString(ResultStr));
	}
}

void URankingWidget::SetDisableAddButton()
{
	AddRanking->SetIsEnabled(false);
}

// FString 배열을 받아와 UI에 랭킹을 표시하는 부분
void URankingWidget::SetRanking(TArray<FString> FileLines)
{
	if (RankingBox)
	{
		RankingBox->ClearChildren();
		for (FString Str : FileLines)
		{
			// Create a new SizeBox to contain the TextBlock
			USizeBox* SizeBox = NewObject<USizeBox>(RankingBox);
			SizeBox->SetWidthOverride(300.f);
			SizeBox->SetHeightOverride(50.f); 
			UTextBlock* PlayerText = NewObject<UTextBlock>(SizeBox);

			FSlateFontInfo FontInfo = PlayerText->Font;
			FontInfo.Size = 40;
			FontInfo.TypefaceFontName = FName(TEXT("Bold"));

			PlayerText->SetFont(FontInfo);
			PlayerText->SetColorAndOpacity(FSlateColor(FLinearColor::White));  // Set the font color
			
			//UE_LOG(LogTemp,Warning,TEXT("%s"),*Str);
			PlayerText->SetText(FText::FromString(Str));
			SizeBox->AddChild(PlayerText);

			RankingBox->AddChild(SizeBox);
		}
	}
}