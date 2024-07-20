// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ExplosionZone.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "CSW/PlayerTankVehicle.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AExplosionZone::AExplosionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(50, 50, 50));

	Explosion = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion"));
	Explosion->SetupAttachment(RootComponent);
	Explosion->bAutoActivate = false;
	
}

// Called when the game starts or when spawned
void AExplosionZone::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AExplosionZone::OnBeginOverlap);
}

// Called every frame
void AExplosionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosionZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerTankVehicle>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("OverLap"));

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion->Template, Explosion->GetComponentLocation(), FRotator::ZeroRotator, FVector(3));

		this->Destroy();
	}
}
