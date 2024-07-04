// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HJS/HpBarWidget.h"

void UHpBarWidget::UpdateHpBar(float HealthPercent)
{
    if (HPProgressBar)
    {
        HPProgressBar->SetPercent(HealthPercent);
    }
}

void UHpBarWidget::UpdateHpText(const FText& HealthText)
{
    if (HPText)
    {
        HPText->SetText(HealthText);
    }
}
