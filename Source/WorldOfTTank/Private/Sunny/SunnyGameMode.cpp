// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyGameMode.h"
//#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyNewTTank.h"
#include "Sunny/SunnyPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

#include "HJS/TotalState.h"
#include "HJS/EndGameWidget.h"
#include "CSW/PlayerTankVehicle.h"




ASunnyGameMode::ASunnyGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// 미니맵 카메라 추가
	MiniMapCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCamera"));
}

void ASunnyGameMode::OnEnemyDie()
{
	TotalStateUI->UpdateTotalStateUI(--EnemyCount);
	if (EnemyCount <= 0)
	{
		bVictory = true;
		EndGame();
	}
}

void ASunnyGameMode::OnPlayerDie()
{
	bVictory = false;
	EndGame();
}




void ASunnyGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	APlayerTankVehicle* Player = Cast<APlayerTankVehicle>(PlayerController->GetPawn());

	PlayerController->bShowMouseCursor = false;

	PlayerController->SetInputMode(FInputModeGameOnly());

	if (BGM)
	{
		
		GetWorldTimerManager().SetTimer(BGMTimerHandle, this, &ASunnyGameMode::VolumeDown, BGMRate, false);
	}

	StartTime = GetWorld()->GetTimeSeconds();
}



void ASunnyGameMode::TotalStateWidgetAdd()
{
	if (TotalStateUIFactory != nullptr)
	{
		TotalStateUI = Cast<UTotalState>(CreateWidget(GetWorld(), TotalStateUIFactory, FName("TotalStateUI")));
		if (TotalStateUI != nullptr)
		{
			//화면에 보이게 
			TotalStateUI->AddToViewport();
			TotalStateUI->UpdateTotalStateUI(EnemyCount);
		}
	}
}

void ASunnyGameMode::VolumeDown()
{
	BGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BGM);
	if (BGMComponent)
	{
		BGMComponent->SetVolumeMultiplier(0.5f);
	}
}

void ASunnyGameMode::RestartGame()
{
	FString MapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName));
}

void ASunnyGameMode::QuitGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
}

void ASunnyGameMode::UpdateRanking(FString Name, float Playtime)
{
	// 이름과 플레이타임을 받아 랭킹에 추가(데이터 테이블 갱신)
	 if (RankingDataTable == nullptr)
	{
		return;
	}

	// 데이터 테이블에 데이터 추가하는 과정
	FRankingData NewRankingData;
	NewRankingData.PlayerName = Name;  // 실제 플레이어 이름을 사용
	NewRankingData.PlayTime = Playtime;
	// RowName 설정
	FName NewRowName = FName(*Name);
	// 데이터 테이블에 Row 추가
	RankingDataTable->AddRow(NewRowName, NewRankingData);
	// 파일 경로 설정
	RankingDataTable->MarkPackageDirty();

	// 패키지 저장
	FString PackagePath = RankingDataTable->GetOutermost()->GetName();
	UPackage* Package = RankingDataTable->GetOutermost();
	FString FilePath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());

	bool bSuccess = UPackage::SavePackage(
		Package,
		RankingDataTable,
		EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
		*FilePath,
		GError,
		nullptr,
		true,
		true,
		SAVE_NoError
	);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("DataTable saved successfully: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save DataTable: %s"), *FilePath);
	}

	// 이후 랭킹 다시 세팅 (UI갱신)
	LoadRankingData();
	ConvertRankingToString();
}

void ASunnyGameMode::LoadRankingData()
{

	if (RankingDataTable == nullptr)
	{
		return;
	}
	// 데이터 테이블의 모든 행을 가져옵니다.
	TArray<FName> RowNames = RankingDataTable->GetRowNames();
	FString ContextString;
	Ranking.Empty();
	Ranking.Reserve(10);

	for (const FName& RowName : RowNames)
	{
		FRankingData* Row = RankingDataTable->FindRow<FRankingData>(RowName, ContextString);
		if (Row)
		{
			FRankingData Data;
			Data.PlayerName = RowName.ToString();
			Data.PlayTime = Row->PlayTime;
			Ranking.Add(Data);
			// 여기서 데이터를 사용하여 UI를 업데이트합니다.
			//UE_LOG(LogTemp, Warning, TEXT("Player: %s, PlayTime: %f"), *Data.PlayerName, Data.PlayTime);

		}
	}

	Ranking.Sort([](const FRankingData& A, const FRankingData& B) {
		return A.PlayTime < B.PlayTime;
	});

	// 상위 10개의 항목만 유지
	if (Ranking.Num() > 5)
	{
		Ranking.SetNum(5);
	}
}

void ASunnyGameMode::EndGame()
{
	if (EndGameUIFactory != nullptr)
	{
		EndGameUI = Cast<UEndGameWidget>(CreateWidget(GetWorld(), EndGameUIFactory, FName("EndGameUI")));
		if (EndGameUI != nullptr)
		{
			//화면에 보이게 
			EndGameUI->AddToViewport();
		}
	}

	TotalStateUI->RemoveFromParent();

	LoadRankingData();
	AccPlayTime = GetWorld()->GetTimeSeconds()-StartTime;
	
	if (bVictory)
	{
		EndGameUI->VictorySetting(AccPlayTime);

	}
	else
	{
		EndGameUI->LoseSetting();
	}


	ConvertRankingToString();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	APlayerTankVehicle* Player = Cast<APlayerTankVehicle>(PlayerController->GetPawn());
	


	PlayerController->bShowMouseCursor = true;

	PlayerController->SetInputMode(FInputModeUIOnly());

	EndGameUI->SetIsFocusable(true);
	EndGameUI->SetUserFocus(GetWorld()->GetFirstPlayerController());


}

void ASunnyGameMode::ConvertRankingToString()
{
	// 랭킹 정보를 이름 + 분:초:밀리초로 변환
	// 분, 초, 밀리초로 나누기
	TArray<FString> RankingText;
	RankingText.Reserve(5);
	int32 Count = 1;
	for (const FRankingData& Rank : Ranking)
	{
		float PlayLog = Rank.PlayTime;
		int32 Minutes = FMath::FloorToInt(PlayLog / 60.0f);
		int32 Seconds = FMath::FloorToInt(PlayLog) % 60;
		int32 Milliseconds = FMath::RoundToInt((PlayLog - FMath::FloorToInt(PlayLog)) * 1000);

		RankingText.Add(FString::Printf(TEXT("%d. %s : %02d:%02d:%03d"),Count++, *Rank.PlayerName, Minutes, Seconds, Milliseconds));
	}
	
	EndGameUI->ResultUISetting(bVictory, RankingText);

}

//void ASunnyGameMode::ActorDied(AActor* DeadActor)
//{
	/*if (DeadActor == TTank)
	{
		TTank->HandleDestruction();
		if (TTanksPlayerController)
		{
			TTanksPlayerController->SetPlayerEnabledState(false);
		}
	}
	else if (ASunnyEnemy* DestroyedEnemy = Cast<ASunnyEnemy>(DeadActor))
	{
		DestroyedEnemy->HandleDestruction();
	}*/
	//}
