// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyTTank.h"
#include "Sunny/SunnyEnemyFSM.h"
#include "Sunny/SunnyGameMode.h"

#include "CSW/PlayerTank.h"

#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


ASunnyEnemy::ASunnyEnemy()
{
	// EnemyFSM 컴포넌트 추가
	Fsm = CreateDefaultSubobject<USunnyEnemyFSM>(TEXT("FSM"));

	// Pawn Move 컴포넌트 추가
	EnemyMove = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
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
	/*TTankGameMode = Cast<ASunnyGameMode>(UGameplayStatics::GetGameMode(this));*/
}



// 몸체 회전
void ASunnyEnemy::RotateTank(FVector LookAtTarget)
{

	if (LookAtTarget == FVector::ZeroVector) {
		return;
	}
	FVector ToTarget = LookAtTarget - BodyMesh->GetComponentLocation();
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	BodyMesh->SetWorldRotation(
		FMath::RInterpTo(BodyMesh->GetComponentRotation(),
			LookAtRotation,
			UGameplayStatics::GetWorldDeltaSeconds(this)
			, 2.f)
	);
}




//  발사 타이머 설정
void ASunnyEnemy::SetFireTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("SetFireTimer()"));
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ASunnyEnemy::CheckFireCondition, FireRate, true);
}

// 발사 타이머 해제
void ASunnyEnemy::ClearFIreTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("ClearFireTimer()"));
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}
	


// 발사할지 여부 확인
void ASunnyEnemy::CheckFireCondition()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckFireCondition()"));
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



///////////////////////////
// 체력바 표시 (추가)
float ASunnyEnemy::GetHealthPercent(float Health, float MaxHealth)
{
	return Health / MaxHealth;
}


// 체력이 0 이면  죽음
void ASunnyEnemy::OnDie()
{
	bDie = true;
	// 머리 날리기
	HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	HeadMesh->SetSimulatePhysics(true);
	HeadMesh->AddImpulse(FVector(0, 0, 2000), NAME_None, true);

	// 종료
	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorTickEnabled(false);
	EnemyMove->StopMovementImmediately();
	ClearFIreTimer();
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

