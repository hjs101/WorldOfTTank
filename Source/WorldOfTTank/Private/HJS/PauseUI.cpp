// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/PauseUI.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Sunny/SunnyGameMode.h"
#include "Components/VerticalBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPauseUI::SetRanking(TArray<FString> FileLines)
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
			FontInfo.Size = 30;
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

void UPauseUI::OnMyRestart()
{
	if (ASunnyGameMode* gm = Cast<ASunnyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->RestartGame();
	}
}

void UPauseUI::OnMyQuit()
{
	if (ASunnyGameMode* gm = Cast<ASunnyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->QuitGame();
	}
}

void UPauseUI::NativeConstruct()
{
	Restart->OnClicked.AddDynamic(this, &UPauseUI::OnMyRestart);
	Quit->OnClicked.AddDynamic(this, &UPauseUI::OnMyQuit);
}

// FString 배열을 받아와 UI에 랭킹을 표시하는 부분