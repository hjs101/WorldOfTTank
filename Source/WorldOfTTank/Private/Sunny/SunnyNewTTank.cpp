// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyNewTTank.h"
#include "Sunny/SunnyNewFSM.h"
#include "Sunny/SunnyGameMode.h"
#include "Sunny/SunnyHealth.h"

#include "CSW/PlayerTankVehicle.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Components/WidgetComponent.h"
#include "Components/progressBar.h"
#include "Components/TextBlock.h"

//#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "ChaosWheeledVehicleMovementComponent.h"



ASunnyNewTTank::ASunnyNewTTank()
{
	// EnemyFSM 컴포넌트 추가
	FSM = CreateDefaultSubobject<USunnyNewFSM>(TEXT("FSM"));

	// Pawn Move 컴포넌트 추가
	EnemyMove = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));

	// Health Bar Widget 컴포넌트 추가
	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HEALTH BAR"));
	HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	HealthComp = CreateDefaultSubobject<USunnyHealth>(TEXT("HealthComp"));

	// Enemy Indicator 컴포넌트 추가
	EnemyIndicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Enemy Indicator"));
	EnemyIndicator->SetupAttachment(GetMesh());
}


void ASunnyNewTTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FSM && InFireRange())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASunnyNewTTank::Tick() ---> InFireRange()"))
		RotateTurret(PlayerTTank->GetActorLocation());
		RotateTank(PlayerTTank->GetActorLocation());
	}
	

	SetBeamLocation();
}


void ASunnyNewTTank::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터가 위를 바라봤을 때 화살표가 안 보이게끔 설정 <-- 왜 안 돼?
	//EnemyIndicator->SetHiddenInGame(true);
	EnemyIndicator->bVisibleInSceneCaptureOnly = true;

	PlayerTTank = Cast<APlayerTankVehicle>(UGameplayStatics::GetPlayerPawn(this, 0));


	if (HealthWidgetComp)
	{
		UUserWidget* HealthWidget = HealthWidgetComp->GetUserWidgetObject();
		if (HealthWidget)
		{
			HealthBar = Cast<UProgressBar>(HealthWidget->GetWidgetFromName(TEXT("HealthBar")));
			CurrentHealthLabel = Cast<UTextBlock>(HealthWidget->GetWidgetFromName(TEXT("CurrentHealthLabel")));
			MaxHealthLabel = Cast<UTextBlock>(HealthWidget->GetWidgetFromName(TEXT("MaxHealthLabel")));

			if (!HealthBar || !CurrentHealthLabel || !MaxHealthLabel)
			{
				UE_LOG(LogTemp, Error, TEXT("HealthBar, CurrentHealthLabel, or MaxHealthLabel is not found in HealthWidget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthWidget is not valid in ASunnyEnemy::BeginPlay"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthWidgetComp is not initialized in ASunnyEnemy::BeginPlay"));
	}

	HealthComp = FindComponentByClass<USunnyHealth>();

	if (HealthComp == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is null in ASunnyEnemy::BeginPlay"));
		return;
	}
	SetHealthPercent(HealthComp->GetHealth(), HealthComp->GetMaxHealth());

	//EnemyIndicator->SetVisibility(true);


	if (FSM)
	{
		UE_LOG(LogTemp, Error, TEXT("FSM component is ok in ASunnyNewTTank::BeginPlay"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FSM component is null in ASunnyNewTTank::BeginPlay"));
	}

	if (EnemyMove == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyMove component is null in ASunnyNewTTank::BeginPlay"));
	}
}


// 탱크 머리 회전 계산
void ASunnyNewTTank::CalcTurretRotation(AActor* TargetActor)
{
	// 타겟이 없으면 리턴
	if (!TargetActor)
	{
		return;
	}

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (nullptr == SkelMesh)
	{
		return;
	}

	// 타겟 방향 벡터 구하기
	FVector ToTarget = TargetActor->GetActorLocation() - SkelMesh->GetSocketLocation(FName("turret_jnt"));
	RotateTurret(ToTarget);
}



//  발사 타이머 설정
void ASunnyNewTTank::SetFireTimer()
{ 
	//UE_LOG(LogTemp, Warning, TEXT("SetFireTimer()"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ASunnyNewTTank::CheckFireCondition, FireRate, true);
}

// 발사 타이머 해제
void ASunnyNewTTank::ClearFIreTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("ClearFireTimer()"));
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	}
}



// 발사할지 여부 확인
void ASunnyNewTTank::CheckFireCondition()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckFireCondition()"));
	if (InFireRange())
	{
		Fire();
	}
}

// 사정 거리 여부 확인 함수
bool ASunnyNewTTank::InFireRange()
{
	//UE_LOG(LogTemp, Warning, TEXT("InFireRange()"));
	if (PlayerTTank)
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerTTank->GetActorLocation());

		if (Distance <= FireRange)
		{
			return true;
		}
	}
	return false;
}



// 체력바 설정
void ASunnyNewTTank::SetHealthPercent(float Health, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Health / MaxHealth);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBar is null"));
	}

	if (CurrentHealthLabel)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		CurrentHealthLabel->SetText(FText::AsNumber(Health, &Opts));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CurrentHealthLabel is null"));
	}

	if (MaxHealthLabel)
	{
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		MaxHealthLabel->SetText(FText::AsNumber(MaxHealth, &Opts));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MaxHealthLabel is null"));
	}
}


// 체력이 0 이면  죽음
void ASunnyNewTTank::OnDie()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDie()"));
	bDie = true;
	// 머리 날리기
	//HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//HeadMesh->SetSimulatePhysics(true);
	//HeadMesh->AddImpulse(FVector(0, 0, 2000), NAME_None, true);

	// 종료
	ClearFIreTimer();
	EnemyMove->StopMovementImmediately();


	// 실행창에 상태 메세지 출력하기
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, TEXT("Enemy 격추 효과"));

	if (bDie)
	{
		// 체력바, 화살표 끄기
		HealthWidgetComp->SetVisibility(false);
		EnemyIndicator->SetVisibility(false);

		// 죽는 순간  내비에 탐지
		GetMesh()->SetCanEverAffectNavigation(true);
	}

	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorTickEnabled(false);
}

// Enemy Delete
void ASunnyNewTTank::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

// 진성 AI 죽음 확인 
bool ASunnyNewTTank::IsDead()
{
	return bDie;
}


// 빔 길이 결정
void ASunnyNewTTank::SetBeamLocation()
{
	FVector Start = ProjectileSpawnPoint->GetComponentLocation();
	FVector ForwardVector = ProjectileSpawnPoint->GetForwardVector();
	FVector End = ForwardVector * 10000.f + Start;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		TraceParams
	);

	if (bHit)
	{
		End = HitResult.Location;

		// Check if the hit actor is the player
		if (HitResult.GetActor()->IsA(APlayerTankVehicle::StaticClass()))
		{
			//UE_LOG(LogTemp, Warning, TEXT("위젯 켜짐"));
			HealthWidgetComp->SetVisibility(true);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("위젯 꺼짐"));
			HealthWidgetComp->SetVisibility(false);
		}
	}


	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 1);

	DrawBeam(Start, End);
}

// 빔 표시
void ASunnyNewTTank::DrawBeam(FVector Start, FVector End)
{
	if (BeamNiagara)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), BeamNiagara, Start, FRotator::ZeroRotator);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVectorParameter(TEXT("Beam End"), End);
		}
	}
}
