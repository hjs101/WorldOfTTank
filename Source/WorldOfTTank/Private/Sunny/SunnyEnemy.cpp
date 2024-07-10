// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyEnemyFSM.h"
#include "Sunny/SunnyGameMode.h"
#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyHealth.h"
#include "CSW/PlayerTank.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Components/WidgetComponent.h"
#include "Components/progressBar.h"
#include "Components/TextBlock.h"

#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h"






ASunnyEnemy::ASunnyEnemy()
{
	// EnemyFSM 컴포넌트 추가
	Fsm = CreateDefaultSubobject<USunnyEnemyFSM>(TEXT("FSM"));

	// Pawn Move 컴포넌트 추가
	EnemyMove = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));

	// Health Bar Widget 컴포넌트 추가
	HealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HEALTH BAR"));
	HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	HealthComp = CreateDefaultSubobject<USunnyHealth>(TEXT("HealthComp"));

	// Enemy Indicator 컴포넌트 추가
	EnemyIndicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Enemy Indicator"));
	EnemyIndicator->SetupAttachment(HeadMesh);
}

void ASunnyEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InFireRange())
	{
		RotateTurret(TTank->GetActorLocation());
	}
	
	SetBeamLocation();
}

void ASunnyEnemy::BeginPlay()
{
	Super::BeginPlay();

	TTank = Cast<APlayerTank>(UGameplayStatics::GetPlayerPawn(this, 0));


	if (HealthWidgetComp)
	{
		UUserWidget* HealthWidget = HealthWidgetComp->GetUserWidgetObject();
		if (HealthWidget)
		{
			HealthBar = Cast<UProgressBar>(HealthWidget->GetWidgetFromName(TEXT("HealthBar")));
			CurrentHealthLabel = Cast<UTextBlock>(HealthWidget->GetWidgetFromName(TEXT("CurrentHealthLabel")));
			MaxHealthLabel = Cast<UTextBlock>(HealthWidget->GetWidgetFromName(TEXT("MaxHealthLabel")));
			// 잘 보면 100,100은생겼는데 체력바는 안생겼죠 응응 ㄱ
			// 이게 왜그러냐면, Health 컴포넌트가 이 에너미의 자식이잖아요 엉
			// 순서가 반대인듯?? 잘은 모르겠지만 비긴플레이가 동시에 아니면 여기가 먼저 생기거나 하는 거 같은데 헬스컴포넌트에서 생성자에서 넣으면 될듯?
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

	//HealthComp = FindComponentByClass<USunnyHealth>();

	if (HealthComp == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is null in ASunnyEnemy::BeginPlay"));
		return;
	}
	SetHealthPercent(HealthComp->GetHealth(), HealthComp->GetMaxHealth());
}


// 몸체 회전
void ASunnyEnemy::RotateTank(FVector LookAtTarget)
{
	if (LookAtTarget == FVector::ZeroVector)
	{
		return;
	}
	FVector ToTarget = LookAtTarget - BodyMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	BodyMesh->SetWorldRotation(FMath::RInterpTo(BodyMesh->GetComponentRotation(), LookAtRotation, UGameplayStatics::GetWorldDeltaSeconds(this), 2.f));
}




//  발사 타이머 설정
void ASunnyEnemy::SetFireTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("SetFireTimer()"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ASunnyEnemy::CheckFireCondition, FireRate, true);
}

// 발사 타이머 해제
void ASunnyEnemy::ClearFIreTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("ClearFireTimer()"));
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	}
}
	


// 발사할지 여부 확인
void ASunnyEnemy::CheckFireCondition()
{
	//UE_LOG(LogTemp, Warning, TEXT("CheckFireCondition()"));
	if (InFireRange())
	{
		Fire();
	}
}

// 사정 거리 여부 확인 함수
bool ASunnyEnemy::InFireRange()
{
	//UE_LOG(LogTemp, Warning, TEXT("InFireRange()"));
	if (TTank)
	{
		float Distance = FVector::Dist(GetActorLocation(), TTank->GetActorLocation());

		if (Distance <= FireRange)
		{
			return true;
		}
	}
	return false;
}



// 체력바 설정
void ASunnyEnemy::SetHealthPercent(float Health, float MaxHealth)
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
void ASunnyEnemy::OnDie()
{
	//UE_LOG(LogTemp, Warning, TEXT("뚝배기!!"));
	bDie = true;
	// 머리 날리기
	HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	HeadMesh->SetSimulatePhysics(true);
	HeadMesh->AddImpulse(FVector(0, 0, 2000), NAME_None, true);

	// 종료
	ClearFIreTimer();
	EnemyMove->StopMovementImmediately();
	

	// 실행창에 상태 메세지 출력하기
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan,TEXT("Enemy 격추 효과"));

	if (bDie)
	{
		// 체력바, 화살표 끄기
		HealthWidgetComp->SetVisibility(false);
		EnemyIndicator->SetVisibility(false);

		// 죽는 순간  내비에 탐지
		BodyMesh->SetCanEverAffectNavigation(true);
		HeadMesh->SetCanEverAffectNavigation(true);
		CapsuleComp->SetSimulatePhysics(false);
	}

	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorTickEnabled(false);
}

// Enemy Delete
void ASunnyEnemy::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

// 진성 AI 죽음 확인 
bool ASunnyEnemy::IsDie()
{
	return bDie;
}


// 빔 길이 결정
void ASunnyEnemy::SetBeamLocation()
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
		if (HitResult.GetActor()->IsA(APlayerTank::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("위젯 켜짐"));
			HealthWidgetComp->SetVisibility(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("위젯 꺼짐"));
			HealthWidgetComp->SetVisibility(false);
		}
	}


	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 1);

	DrawBeam(Start, End);
}

// 빔 표시
void ASunnyEnemy::DrawBeam(FVector Start, FVector End)
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

