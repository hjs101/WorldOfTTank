// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIProjecttile_1.generated.h"

UCLASS()
class WORLDOFTTANK_API AAIProjecttile_1 : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIProjecttile_1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	// 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "combat")
	UStaticMeshComponent* ProjecttileComp;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjecttileMovement;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
