// Fill out your copyright notice in the Description page of Project Settings.


#include "HJS/Obstacle.h"

// Sets default values
AObstacle::AObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Obj Mesh Comp"));
    RootComponent = MeshComp;

    Tags.Add(FName("Obstacle"));
}

TArray<FVector> AObstacle::GetLocationArr()
{
    TArray<FVector> ResponseLocations;

    for (USceneComponent* PointObj : HiddenPointArr) {
        ResponseLocations.Add(PointObj->GetComponentLocation());
    }

    return ResponseLocations;
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

    UE_LOG(LogTemp, Warning, TEXT("Number of components added to array: %d"), HiddenPointArr.Num());
}

