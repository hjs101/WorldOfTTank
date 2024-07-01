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
}

// Called when the game starts or when spawned
void AFallingObj::BeginPlay()
{
	Super::BeginPlay();
	MeshComp->OnComponentHit.AddDynamic(this, &AFallingObj::OnHit);
	
}

// Called every frame
void AFallingObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFallingObj::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && (OtherActor != this) && Cast<AAITank_1>(OtherActor))
	{
		FVector Impulse = Hit.ImpactNormal * 10000.0f;
		MeshComp->AddImpulseAtLocation(Impulse, Hit.ImpactPoint);
	}
}

