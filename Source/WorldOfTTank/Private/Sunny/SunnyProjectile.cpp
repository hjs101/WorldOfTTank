// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyProjectile.h"

#include "CSW/PlayerTankVehicle.h"
#include "HJS/AITankCPU_1.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"


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


	// Hit 이펙트 나이아가라 컴포넌트 추가
	//HitNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitNiagara"));
	//HitNiagara->SetupAttachment(ProjectileMesh);
	//HitNiagara->bAutoActivate = false; // 처음에는 비활성 상태로 설정
	//HitNiagara->SetRelativeScale3D(FVector(1.f)); // 필요에 따라 크기 조정

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
	if (MyOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SunnyProjectile::OnHit()  --> NO MyOwner"));
		return;
	}

	auto MyOwnerInstigator = MyOwner->GetInstigatorController();
	auto DamageTypeClass = UDamageType::StaticClass();


	// Player일때
	APlayerTankVehicle* PlayerTank = Cast<APlayerTankVehicle>(OtherActor);
	if (PlayerTank != nullptr)
	{
		PlayerTank->SetPlayerTankDamage(Damage);
	}

	AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OtherActor);

	if (AITank != nullptr) 
	{
		AITank->HealthDown(Damage);
	}

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	}

	// Hit point 
	FVector Location = ProjectileMesh->GetComponentLocation();
	FRotator Rotation = ProjectileMesh->GetComponentRotation();

	if (HitNiagara) {
		// ProjectileSpawnPoint의 위치와 회전에 Niagara 이펙트 재생
		//HitNiagara->SetWorldLocationAndRotation(Location, Rotation);
		//HitNiagara->Activate(); // Niagara 이펙트 활성화
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, Hit.Location, FRotator::ZeroRotator, FVector(1));
	}

	/*if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}*/

	Destroy();

}

