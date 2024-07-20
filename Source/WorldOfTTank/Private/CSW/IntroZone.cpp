// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/IntroZone.h"

#include "Components/BoxComponent.h"
#include "CSW/PlayerTankVehicle.h"

// Sets default values
AIntroZone::AIntroZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(50, 50, 50));
}

// Called when the game starts or when spawned
void AIntroZone::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AIntroZone::OnBeginOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AIntroZone::OnEndOverlap);
}

// Called every frame
void AIntroZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIntroZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerTankVehicle* player = Cast<APlayerTankVehicle>(OtherActor);
	if (player)
	{
		player->EndIntro();
		this->Destroy();
	}
}

void AIntroZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

