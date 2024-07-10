// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Projectile.h"
#include "Sunny/SunnyEnemy.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "HJS/AITankCPU_1.h"
#include "GameFramework/DamageType.h"
#include "HJS/FractureWall.h"
#include "HJS/Obstacle.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move Comp"));
	MoveComp->InitialSpeed = 13000;
	MoveComp->MaxSpeed = 13000;

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/CSW/Asset/SM_Projectile.SM_Projectile'"));
	if(TempMesh.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(TempMesh.Object);
		ProjectileMesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	}
	Explosion = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion"));
	Explosion->SetupAttachment(RootComponent);
	Explosion->bAutoActivate = false;
	Explosion->SetActive(false);
	ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/HJS/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Big_B.P_Explosion_Big_B'"));
	if (ParticleAsset.Succeeded())
	{
		Explosion->SetTemplate(ParticleAsset.Object);
		Explosion->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::AddDecalAtLocation(const FVector& Location, const FVector& Normal) const
{
	
    UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
        this,
        DecalMaterial, // 대포 자국에 사용할 마테리얼
        FVector(30.0f, 50.0f, 50.0f), // 데칼 크기
        Location,
        Normal.Rotation()
    );

    // 데칼의 수명 설정 (필요에 따라 설정)
    if (Decal)
        Decal->SetLifeSpan(10.0f); // 예: 10초 후에 사라지도록 설정
}



void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Cast<APawn>(OtherActor))
		AddDecalAtLocation(Hit.Location, Hit.Normal);
	
	//AObstacle* ObstacleComp = Cast<AObstacle>(OtherActor);
	//if (ObstacleComp != nullptr) {
	//	ObstacleComp->AddDecalAtLocation(Hit.ImpactPoint, Hit.ImpactNormal);
	//}

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, TEXT("적 타격 효과"));
	Explosion->Activate();
	// 부서지는 장애물일때
	AFractureWall* FractureComp = Cast<AFractureWall>(OtherActor);

	if (FractureComp != nullptr) 
	{
		if (!FractureComp->HpDown())
		{
			FTransform HitTransform;
			HitTransform.SetLocation(Hit.Location);
			HitTransform.SetRotation(FRotator(GetActorRotation().Pitch - 90.f, GetActorRotation().Yaw, GetActorRotation().Roll).Quaternion());
			HitTransform.SetScale3D(FVector(1.f, 1.f, 1.f));
			GetWorld()->SpawnActor<AActor>(MasterFieldClass, HitTransform);
			
			FractureComp->SetDestroyTimer();
		}
	}
	// AICPU일때
	AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OtherActor);

	if (AITank != nullptr) 
	{
		AITank->HealthDown(50);
		AITank->SetBlackBoardTarget(OtherActor);
	}

	// SunnyEnemy일때
	AActor* MyOwner = GetOwner();
	if (MyOwner != nullptr) 
	{
		AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
		if (MyOwnerInstigator != nullptr) 
		{
			UClass* DamageTypeClass = UDamageType::StaticClass();
			ASunnyEnemy* SunnyEnemy = Cast<ASunnyEnemy>(OtherActor);
			if (SunnyEnemy != nullptr)
			{
				UGameplayStatics::ApplyDamage(OtherActor, 50.f, MyOwnerInstigator, this, DamageTypeClass);
			}
		}
	}


	// Destroy();
}

