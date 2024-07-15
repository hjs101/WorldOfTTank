// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyRealTTank.h"
//#include "Sunny/SunnyEnemyFSM.h"
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

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "ChaosWheeledVehicleMovementComponent.h"



ASunnyRealTTank::ASunnyRealTTank()
{
	// EnemyFSM 컴포넌트 추가
	//Fsm = CreateDefaultSubobject<USunnyEnemyFSM>(TEXT("FSM"));

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



void ASunnyRealTTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InFireRange())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Tick()->InFireRange()"))
		RotateTurret(PlayerTTank->GetActorLocation());
	}

	SetBeamLocation();
}

void ASunnyRealTTank::RotateTurret(FVector LookAtTarget)
{
	//UE_LOG(LogTemp, Warning, TEXT("RotateTurret()"))
	//if (LookAtTarget == FVector::ZeroVector)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("RotateTurret()->NO LookAtTarget"))
	//	return;
	//}
	//UE_LOG(LogTemp, Warning, TEXT("RotateTurret()-> OK LookAtTarget"))

	//USkeletalMeshComponent* SkelMesh = GetMesh();
	//UE_LOG(LogTemp, Warning, TEXT("======Set SkelMesh==============="))

	//if (SkelMesh == nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("RotateTurret()->NO SkelMesh"))
	//	return;
	//}
	//UE_LOG(LogTemp, Warning, TEXT("RotateTurret()->OK SkelMesh"))

	//// Get the location of the turret socket
	//FVector TurretLocation = SkelMesh->GetSocketLocation(FName("turret_jnt"));
	//UE_LOG(LogTemp, Warning, TEXT("TurretLocation: X=%f, Y=%f, Z=%f"), TurretLocation.X, TurretLocation.Y, TurretLocation.Z);

	//// Calculate the direction to the target
	//FVector ToTarget = PlayerTTank->GetActorLocation() - TurretLocation;
	//FRotator ToTargetRotation = ToTarget.Rotation();
	//UE_LOG(LogTemp, Warning, TEXT("ToTargetRotation: X=%f, Y=%f, Z=%f"), ToTargetRotation.Roll, ToTargetRotation.Pitch, ToTargetRotation.Yaw);

	//// Get the current rotation of the turret
	//FRotator CurrentRotation = SkelMesh->GetSocketRotation(FName("turret_jnt"));
	//UE_LOG(LogTemp, Warning, TEXT("CurrentRotation: X=%f, Y=%f, Z=%f"), CurrentRotation.Roll, CurrentRotation.Pitch, CurrentRotation.Yaw);

	//// Calculate the difference in yaw between the current rotation and the target rotation
	//float YawDifference = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, ToTargetRotation.Yaw);
	//UE_LOG(LogTemp, Warning, TEXT("YawDifference: %f"), YawDifference);

	//// Normalize the yaw difference to be within -1 to 1
	//float YawInput = FMath::Clamp(YawDifference / 180.0f, -1.0f, 1.0f);
	//UE_LOG(LogTemp, Warning, TEXT("YawInput: %f"), YawInput);


	//// Get the vehicle movement component and set the yaw input
	//UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	//if (VehicleMovement)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("SetYawInput()"));

	//	VehicleMovement->SetYawInput(YawInput);
	//}
	


}

void ASunnyRealTTank::RotateBarrel(FVector LookAtTarget)
{

}

void ASunnyRealTTank::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerTTank = Cast<APlayerTank>(UGameplayStatics::GetPlayerPawn(this, 0));


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

	//HealthComp = FindComponentByClass<USunnyHealth>();

	if (HealthComp == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is null in ASunnyEnemy::BeginPlay"));
		return;
	}
	SetHealthPercent(HealthComp->GetHealth(), HealthComp->GetMaxHealth());

	//EnemyIndicator->SetVisibility(true);
}



//  발사 타이머 설정
void ASunnyRealTTank::SetFireTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("SetFireTimer()"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ASunnyRealTTank::CheckFireCondition, FireRate, true);
}

// 발사 타이머 해제
void ASunnyRealTTank::ClearFIreTimer()
{
	//UE_LOG(LogTemp, Warning, TEXT("ClearFireTimer()"));
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	}
}



// 발사할지 여부 확인
void ASunnyRealTTank::CheckFireCondition()
{
	//UE_LOG(LogTemp, Warning, TEXT("CheckFireCondition()"));
	if (InFireRange())
	{
		Fire();
	}
}

// 사정 거리 여부 확인 함수
bool ASunnyRealTTank::InFireRange()
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
void ASunnyRealTTank::SetHealthPercent(float Health, float MaxHealth)
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
void ASunnyRealTTank::OnDie()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnDie()"));
	bDie = true;
	// 머리 날리기
	//HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//HeadMesh->SetSimulatePhysics(true);
	//HeadMesh->AddImpulse(FVector(0, 0, 2000), NAME_None, true);

	// 종료
	ClearFIreTimer();
	EnemyMove->StopMovementImmediately();


	// 실행창에 상태 메세지 출력하기
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, TEXT("Enemy 격추 효과"));

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
void ASunnyRealTTank::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

// 진성 AI 죽음 확인 
bool ASunnyRealTTank::IsDie()
{
	return bDie;
}


// 빔 길이 결정
void ASunnyRealTTank::SetBeamLocation()
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
void ASunnyRealTTank::DrawBeam(FVector Start, FVector End)
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