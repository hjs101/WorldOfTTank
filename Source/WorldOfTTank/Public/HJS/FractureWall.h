// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FractureWall.generated.h"

UCLASS()
class WORLDOFTTANK_API AFractureWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFractureWall();

	UFUNCTION()
	bool HpDown();
	
	UFUNCTION()
	void SetDestroyTimer();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 지오메트리 컴포넌트 선언하기
	UPROPERTY(VisibleAnywhere, Category = "Fractured Mesh")
    UGeometryCollectionComponent* FracturedMeshComp;

	// 체력 설정
	UPROPERTY(VisibleAnywhere, Category = "Fracture")
	int WallHP = 1;

	// 부서졌는지 체크
	bool bBroken = false;

	// 머티리얼 인터페이스 포인터
    UPROPERTY(EditAnywhere)
    UMaterialInterface* FadeMaterial;

	// 머티리얼 인스턴스
	UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;
	// FadeOut 시간 설정
    UPROPERTY(EditAnywhere)
    float FadeDuration = 5.f;
	// 현재 진행 시간
	float CurrentTime = 0.f;
	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UFUNCTION()
	void DestroySelf();

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};
