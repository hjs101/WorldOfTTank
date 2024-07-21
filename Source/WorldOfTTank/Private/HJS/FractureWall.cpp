// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/FractureWall.h"
#include "TimerManager.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values
AFractureWall::AFractureWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FracturedMeshComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("FracturedMeshComponent"));
	SetRootComponent(FracturedMeshComp);

	FracturedMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	FracturedMeshComp->SetNotifyRigidBodyCollision(true);  // Hit 이벤트 활성화
}

// Called when the game starts or when spawned
void AFractureWall::BeginPlay()
{
	Super::BeginPlay();
	

	if (FadeMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(FadeMaterial, this);
		if (DynamicMaterial)
		{
			UGeometryCollectionComponent* Mesh = Cast<UGeometryCollectionComponent>(RootComponent);
			if (Mesh)
			{
				Mesh->SetMaterial(0, DynamicMaterial);
			}
		}
	}
	// 충돌 이벤트 바인딩
	FracturedMeshComp->OnComponentHit.AddDynamic(this, &AFractureWall::OnComponentHit);

}

// Called every frame
void AFractureWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBroken && DynamicMaterial)
	{
		CurrentTime += DeltaTime;
		float Opacity = FMath::Clamp(1.0f - (CurrentTime / FadeDuration), 0.0f, 1.0f);
		DynamicMaterial->SetScalarParameterValue(TEXT("Fade"), Opacity);

		if (CurrentTime >= FadeDuration)
		{
			bBroken = false;
			// 오브젝트를 사라지게 설정
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
	}

}

bool AFractureWall::HpDown()
{
	WallHP--;

	if (WallHP > 0) {
		return true;
	}
	else {
		return false;
	}
}

void AFractureWall::SetDestroyTimer()
{
	if(!bBroken){
		bBroken = true;
		CurrentTime = 0.0f;
		UGameplayStatics::PlaySound2D(GetWorld(), DestroySound);
		//GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AFractureWall::DestroySelf, 5.0f, false);
	}
}

void AFractureWall::DestroySelf()
{
	Destroy();
}

void AFractureWall::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Hit detected on Geometry Collection!"));
	// 충돌한 액터가 탱크인지 확인
	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		SetDestroyTimer();
		SetActorEnableCollision(false);
	}
	//	// 탱크의 이동 방향과 속도 가져오기
	//	//FVector TankVelocity = OtherActor->GetVelocity();
	//	FVector TankDirection = OtherActor->GetActorForwardVector().GetSafeNormal(); // 방향 벡터 노멀라이즈

	//	// 힘의 크기 설정 (탱크 속도에 비례하게 설정)
	//	float ForceMagnitude = 5000.f; // ForceMultiplier는 적절히 설정

	//	// 충돌한 조각에 힘을 가함
	//	FVector Force = TankDirection * ForceMagnitude;
	//	HitComponent->AddImpulse(Force);
	//}
}
