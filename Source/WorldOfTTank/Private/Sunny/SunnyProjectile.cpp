// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "CSW/Tank.h"
#include "HJS/AITankCPU_1.h"
// Sets default values
ASunnyProjectile::ASunnyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 1300.f;
	ProjectileMovementComponent->InitialSpeed = 1300.f;

}

// Called when the game starts or when spawned
void ASunnyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ASunnyProjectile::OnHit);
	/*if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}*/
}

// Called every frame
void ASunnyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASunnyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//로그 출력
	/*UE_LOG(LogTemp, Warning, TEXT("OnHit"));
	UE_LOG(LogTemp, Warning, TEXT("HitComp: %s"), *HitComp->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *OtherActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("OtherComp: %s"), *OtherComp->GetName());*/
	
	auto MyOwner = GetOwner();
	if (MyOwner == nullptr) return;

	auto MyOwnerInstigator = MyOwner->GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();


	// Player일때
	ATank* PlayerTank = Cast<ATank>(OtherActor);
	if (PlayerTank != nullptr)
	{
		PlayerTank->SetPlayerTankDamage(50.0);
	}

	AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OtherActor);

	if (AITank != nullptr)
	{
		AITank->HealthDown(50);
	}

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
		Destroy();
	}

}

