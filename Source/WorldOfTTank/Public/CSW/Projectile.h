// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class WORLDOFTTANK_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void	AddDecalAtLocation(const FVector& Location, const FVector& Normal) const;
	
private:
    UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* MoveComp;

	UFUNCTION()
	void	OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere, Category="Decal")
	UMaterial* DecalMaterial = nullptr;

	UPROPERTY(EditAnywhere,  Category="Combat")
	UClass* MasterFieldClass;
};
