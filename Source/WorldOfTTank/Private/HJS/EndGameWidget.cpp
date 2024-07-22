// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/EndGameWidget.h"
#include "HJS/MissionResultUI.h"
#include "HJS/RankingWidget.h"

// 이겼을 때 UI 세팅
void UEndGameWidget::VictorySetting(float ClearTime)
{
	// 1. 미션 성공 창 페이드 인.
	VictoryUI->SetVisibility(ESlateVisibility::Visible);
	VictoryUI->PlayAnim();
	// 2. 페이드 인이 끝나는 시간과 동시에 UI 창 띄우기
	GetWorld()->GetTimerManager().SetTimer(FadeInTimerHandle, this, &UEndGameWidget::ResultUIToVisible, 3.f, true);
	RankingUI->SetResultTime(ClearTime);
}

void UEndGameWidget::LoseSetting()
{
	// 1. 미션 실패 창 페이드인.
	LoseUI->SetVisibility(ESlateVisibility::Visible);
	LoseUI->PlayAnim();
	// 2. 페이드 인이 끝나는 시간과 동시에 UI 창 띄우기
	GetWorld()->GetTimerManager().SetTimer(FadeInTimerHandle, this, &UEndGameWidget::ResultUIToVisible, 3.f, true);
	// Add 버튼 비활성화
	RankingUI->SetDisableAddButton();
	RankingUI->LoseSet();
}

void UEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VictoryUI->SetVisibility(ESlateVisibility::Hidden);
	LoseUI->SetVisibility(ESlateVisibility::Hidden);
	RankingUI->SetVisibility(ESlateVisibility::Hidden);
}

void UEndGameWidget::ResultUISetting(bool bVictory, TArray<FString> Ranking)
{
	RankingUI->SetRanking(Ranking);
}

void UEndGameWidget::ResultUIToVisible()
{
	RankingUI->SetVisibility(ESlateVisibility::Visible);
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetPause(true);
}
