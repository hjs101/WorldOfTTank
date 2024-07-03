// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/AITank_1.h"
#include "HJS/FallingObj.h"

// Sets default values
AFallingObj::AFallingObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeMesh"));
	RootComponent = MeshComp;

	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetNotifyRigidBodyCollision(true);

	FallImpulse = 1000.0f;
	TopOffset = FVector(0.0f, 0.0f, 800.0f); // 기본 오프셋 값, 필요에 따라 조정 가능
}

// Called when the game starts or when spawned
void AFallingObj::BeginPlay()
{
	Super::BeginPlay();
	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AFallingObj::OnOverlapBegin);
	
}

// Called every frame
void AFallingObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFallingObj::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bFalling && OtherActor && Cast<AAITank_1>(OtherActor))
	{
		bFalling = true;
		FVector Impulse = FVector(1.0f, 0.0f, 0.0f) * FallImpulse;

		// 나무의 윗부분 위치 계산
		FVector TopLocation = MeshComp->GetComponentLocation() + TopOffset;

		// 나무의 윗부분에 임펄스 부여
		MeshComp->AddImpulseAtLocation(Impulse, TopLocation);
	}
}
