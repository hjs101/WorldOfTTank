// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/Obstacle.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

// Sets default values
AObstacle::AObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Obj Mesh Comp"));
    RootComponent = MeshComp;

    Tags.Add(FName("Obstacle"));

    ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("/Script/Engine.Material'/Game/HJS/Materials/DecalMaterial.DecalMaterial'"));
    if (TempMat.Succeeded())
    {
        DecalMaterial = TempMat.Object;
    }
    
}

TArray<FVector> AObstacle::GetLocationArr()
{
    TArray<FVector> ResponseLocations;
    ResponseLocations.Empty(HiddenPointArr.Num() - 1);

    for (USceneComponent* PointObj : HiddenPointArr) {
        ResponseLocations.Add(PointObj->GetComponentLocation());
    }

    return ResponseLocations;
}

void AObstacle::AddDecalAtLocation(FVector Location, FVector Normal)
{
    // 데칼 생성에 필요한 파라미터 설정
    UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
        this,
        DecalMaterial, // 대포 자국에 사용할 마테리얼
        FVector(100.f, 100.f, 100.f), // 데칼 크기
        Location,
        Normal.Rotation()
    );

    // 데칼의 수명 설정 (필요에 따라 설정)
    if (Decal)
    {
        Decal->SetLifeSpan(10.0f); // 예: 10초 후에 사라지도록 설정
    }
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	
    // Find all components of the specified type (e.g., UStaticMeshComponent)
    TArray<USceneComponent*> FoundComponents;
    GetComponents(FoundComponents);

    // Add found components to the array
    for (USceneComponent* Component : FoundComponents)
    {
        HiddenPointArr.Add(Component);
    }

}

