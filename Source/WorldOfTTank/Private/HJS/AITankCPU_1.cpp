// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Perception/PawnSensingComponent.h"
#include "HJS/AITankPlayer_1.h"
#include "HJS/AITankCPU_1.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HJS/AITankController_1.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAITankCPU_1::AAITankCPU_1()
{
	// AI컨트롤러 지정
	AIControllerClass = AAITankController_1::StaticClass();
	// Pawn이 생성될 때 자동으로 AIController 할당하도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Create and initialize the Pawn Sensing Component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

}

void AAITankCPU_1::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DetectRateTimerHandle, this, &AAITankCPU_1::CheckDistance, DetectRate, true);
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAITankCPU_1::OnSeePawn);
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	ai = Cast<AAITankController_1>(GetController());
}

// Called every frame
void AAITankCPU_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 일정 거리 이상 벗어났는 지 확인하는 함수
void AAITankCPU_1::CheckDistance()
{
	// AIController 불러오기
	AAITankController_1* AIController = Cast<AAITankController_1>(GetController());
	if (AIController == nullptr) 
	{
		// 컨트롤러 불러오기 실패
		return;
	}
	// 블랙보드 불러오기
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (BlackboardComp == nullptr) 
	{
		return;
	}

	// 타겟 플레이어 호출
	APawn* TargetPlayer = Cast<APawn>(BlackboardComp->GetValueAsObject(FName("TargetPlayer")));
	// 블랙보드에 값이 존재하면
	if (TargetPlayer != nullptr) 
	{
		float Distance = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());
		if (Distance > DetectRange) {
			BlackboardComp->ClearValue("TargetPlayer");
		}
	}

	// 타겟 CPU 호출
	APawn* TargetCPU = Cast<APawn>(BlackboardComp->GetValueAsObject(FName("TargetCPU")));
	if (TargetCPU != nullptr)
	{
		float Distance = FVector::Dist(GetActorLocation(), TargetCPU->GetActorLocation());
		if (Distance > DetectRange) {
			BlackboardComp->ClearValue("TargetCPU");
		}
	}
	return;
}	

// 사정거리 확인하는 함수
bool AAITankCPU_1::InFireRange(APawn* OtherActor)
{
	// 탱크까지의 거리 구하기
	if (OtherActor)
	{
		// 탱크가 사정거리에 있는지 확인
		float Distance = FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());
		// 사정거리에 있다면 True 리턴
		if (Distance <= FireRange)
		{
			return true;
		}
	}
	return false;
}

bool AAITankCPU_1::GetFireState()
{
	return bReadyFire;
}

// 레이저 쏴서 일직선 상에 있는지 확인하는 함수
bool AAITankCPU_1::HasLineOfSightToTarget(AActor* TargetActor) const
{
	// 타겟이 없으면 false 리턴
	if (!TargetActor)
	{
		return false;
	}
	// 시작 위치는 포신의 끝 점으로 하고 싶은데....
	FVector StartLocation = ProjecttileSpawnPoint->GetComponentLocation();
	// 종료 위치는 타겟의 Vector
	FVector EndLocation = TargetActor->GetActorLocation();

	// HitResult 선언
	FHitResult HitResult;
	// CollisionQueryParams 선언
	FCollisionQueryParams CollisionParams;
	// 자기자신은 무시
	CollisionParams.AddIgnoredActor(this);
	// 타겟도 무시
	CollisionParams.AddIgnoredActor(TargetActor);

	// 라인트레이싱 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility, // 가시성 채널 사용
		CollisionParams
	);

	return bHit;
}

// 다 회전 됐는 지 체크하는 함수
bool AAITankCPU_1::IsTurretRotationComplete(AActor* TargetActor) const
{	
	// 타겟이 없으면 돌리면 안되니 true 리턴
	if (!TargetActor)
	{
		return true;
	}

	// 타겟 방향 벡터 구하기
	FVector ToTarget = TargetActor->GetActorLocation() - HeadMesh->GetComponentLocation();
	FRotator ToTargetRotation = ToTarget.Rotation();
	FRotator MyRotation = HeadMesh->GetComponentRotation();
	//1. yaw 각도는 무조건 일치 해야함. // 아예 안될수도 있으니 오차범위 +-2도정도 생각해두기
	if (abs(ToTargetRotation.Yaw - MyRotation.Yaw) > 2) {
		return false;
	}
	MyRotation = BarrelMesh->GetComponentRotation();
	// 2. 고도 확인하기
	if (abs(ToTargetRotation.Pitch - MyRotation.Pitch) > 2) {

		// 2-1. 피치가 다르면서 한계고도 안쪽일 경우 false
		if (ToTargetRotation.Pitch < UpLimit && ToTargetRotation.Pitch > DownLimit) {
			return false;
		}

	}
	return true;
}

// Pawn 감지 이벤트 콜백 함수
void AAITankCPU_1::OnSeePawn(APawn* Pawn)
{
	AAITankController_1* AIController = Cast<AAITankController_1>(GetController());

	if (AIController == nullptr)
	{
		return;
	}
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}
	if (Cast<AAITankPlayer_1>(Pawn)) 
	{
		BlackboardComp->SetValueAsObject(FName("TargetPlayer"), Pawn);
	}
	else 
	{
		BlackboardComp->SetValueAsObject(FName("TargetCPU"), Pawn);
	}
	
}

void AAITankCPU_1::Fire()
{
	// 안전장치, Fire함수가 이상하게 호출됐을 때를 대비
	if(bReadyFire){
		Super::Fire();
		bReadyFire = false;
		GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AAITankCPU_1::FireReady, FireRate, false);
	}
}

// 공격 유효지점 찾기
FVector AAITankCPU_1::FindValidAttackPosition(const AActor* TargetActor)
{


	// 시작위치 불러오기
	FVector StartLocation = GetActorLocation();

	if (TargetActor == nullptr) {
		return StartLocation;
	}
	FVector DestLocation = StartLocation;

	// 라인 트레이스용 변수
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(TargetActor);
	// 샘플링 지점 설정
	float SampleRadius = 1000.f; // 샘플링 반경
	int32 NumSamples = 36; // 샘플링할 지점 수 (360도를 기준으로 10도 간격)
	
	for (int32 i = 0; i < NumSamples; i++)
	{
		// 각도를 기준으로 샘플링 지점 계산
		float Angle = (360.0f / NumSamples) * i;
		FVector SamplePoint = StartLocation + SampleRadius * FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0);
		
		// 라인 트레이스 수행
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			SamplePoint,
			TargetActor->GetActorLocation(),
			ECC_Visibility,
			CollisionParams
		);
		// 충돌이 없고 네비메시에서 유효한 지점인지 확인
		if (!bHit)
		{
			NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (NavSys == nullptr) 
			{
				return StartLocation;
			}

			FPathFindingQuery query;
			FAIMoveRequest req;
			req.SetAcceptanceRadius(3);
			req.SetGoalLocation(SamplePoint);
			ai->BuildPathfindingQuery(req, query);
			FPathFindingResult result = NavSys->FindPathSync(query);
			// 지금 이 if문이 잘 작동을 안함.
			
			if (result.Result == ENavigationQueryResult::Success)
			{
				DestLocation = SamplePoint;
				DrawDebugSphere(GetWorld(), SamplePoint, 12.0f, 12, FColor::Green, false, 3.0f);
				// 타겟과의 거리가 현재 위치보다 더 가까워지지 않는지 체크
				float CurrentDistance = FVector::Dist(StartLocation, TargetActor->GetActorLocation());
				float NewDistance = FVector::Dist(SamplePoint, TargetActor->GetActorLocation());

				// 해당 메인 타겟이 사정거리(FireRange) 내에 있는지 체크
				if (NewDistance <= FireRange && NewDistance >= CurrentDistance)
				{
					// 유효한 위치를 찾았을 때 DestLocation 갱신 및 반환
					DestLocation = SamplePoint;
					return DestLocation;
				}
			}
		}
	}

	return DestLocation;

}

FVector AAITankCPU_1::FindValidAttackRange(const AActor* TargetActor)
{


	// 시작위치 불러오기
	FVector StartLocation = GetActorLocation();

	if (TargetActor == nullptr) {
		return StartLocation;
	}
	FVector DestLocation = StartLocation;
	// 샘플링 지점 설정
	// 샘플링 반경 = 타겟 - 나의 거리에서 사정거리만큼 거리를 빼고, 여기에 일정 텀(300)을 더하기.
	float SampleRadius = FVector::Dist(StartLocation,TargetActor->GetActorLocation()) - FireRange + 200.f; 
	int32 NumSamples = 36; // 샘플링할 지점 수 (360도를 기준으로 10도 간격)
	float MinDist = FVector::Dist(StartLocation, TargetActor->GetActorLocation());
	for (int32 i = 0; i < NumSamples; i++)
	{
		// 각도를 기준으로 샘플링 지점 계산
		float Angle = (360.0f / NumSamples) * i;
		FVector SamplePoint = StartLocation + SampleRadius * FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0);
		// 충돌이 없고 네비메시에서 유효한 지점인지 확인
		NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSys == nullptr)
		{
			return StartLocation;
		}

		FPathFindingQuery query;
		FAIMoveRequest req;
		req.SetAcceptanceRadius(3);
		req.SetGoalLocation(SamplePoint);
		ai->BuildPathfindingQuery(req, query);
		FPathFindingResult result = NavSys->FindPathSync(query);
		// 지금 이 if문이 잘 작동을 안함.
		if (result.Result == ENavigationQueryResult::Success)
		{
			DrawDebugSphere(GetWorld(), SamplePoint, 12.0f, 12, FColor::Green, false, 3.0f);
			// 타겟과의 거리가 현재 위치보다 더 가까워지지 않는지 체크
			float NewDistance = FVector::Dist(SamplePoint, TargetActor->GetActorLocation());

			// 해당 메인 타겟이 가까워지는지
			if (MinDist > NewDistance)
			{
				// 유효한 위치를 찾았을 때 DestLocation 갱신 및 반환
				DestLocation = SamplePoint;
				MinDist = NewDistance;
			}
		}
	}

	return DestLocation;

}

void AAITankCPU_1::FireReady()
{
	bReadyFire = true;
}
