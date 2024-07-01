// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallingObj.generated.h"

UCLASS()
class WORLDOFTTANK_API AFallingObj : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFallingObj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;


    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    UPROPERTY(EditAnywhere, Category = "Physics")
    float FallImpulse;

    UPROPERTY(EditAnywhere, Category = "Physics")
    FVector TopOffset; // 나무의 윗부분 위치 오프셋

	bool bFalling = false;
};
