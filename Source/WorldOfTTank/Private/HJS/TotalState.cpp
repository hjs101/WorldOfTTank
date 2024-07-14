// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/TotalState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
void UTotalState::UpdateTotalStateUI(int32 UpdateEnemy)
{
	
	if (UpdateEnemy == 2)
	{
		FString Text = FString::Printf(TEXT("Enemy Tank : %d"), UpdateEnemy);
		EnemyCount->SetText(FText::FromString(Text));
	}
	else if (UpdateEnemy == 1)
	{
		Enemy_1->SetVisibility(ESlateVisibility::Hidden);
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Enemy_2->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(FVector2D(0.0, CanvasSlot->GetPosition().Y));
		}
		FString Text = FString::Printf(TEXT("Enemy Tank : %d"), UpdateEnemy);
		EnemyCount->SetText(FText::FromString(Text));

	}
	else
	{
		Enemy_1->SetVisibility(ESlateVisibility::Hidden);
		Enemy_2->SetVisibility(ESlateVisibility::Hidden);
		FString Text = "Mission Complete :)";
		EnemyCount->SetText(FText::FromString(Text));
	}

}