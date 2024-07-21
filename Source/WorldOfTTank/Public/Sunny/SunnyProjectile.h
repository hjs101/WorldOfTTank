// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunnyProjectile.generated.h"

UCLASS()
class WORLDOFTTANK_API ASunnyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASunnyProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	 
private: 
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UStaticMeshComponent* ProjectileMesh; 

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float Damage = 50.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// NiagaraSystem
	UPROPERTY(EditAnyWhere, Category = "Effect")
	class UNiagaraSystem* HitNiagara;

	// 미사일 부딪히는 소리
	//UPROPERTY(EditAnywhere, Category = "Sound")
	//class USoundBase* HitSound;

};
