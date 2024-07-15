// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/AIProjecttile_1.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "HJS/AITankCPU_1.h"
#include "HJS/Obstacle.h"
#include "CSW/Tank.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "HJS/FractureWall.h"
#include "Sunny/SunnyNewTTank.h"
#include "Particles/ParticleSystemComponent.h"
#include "CSW/PlayerTankVehicle.h"
// Sets default values
AAIProjecttile_1::AAIProjecttile_1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjecttileComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projecttile Mesh"));
	SetRootComponent(ProjecttileComp);

	ProjecttileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projecttile Movement"));
	ProjecttileMovement->InitialSpeed = 10.f;
	ProjecttileMovement->MaxSpeed = 50.f;

	static ConstructorHelpers::FClassFinder<UObject> MasterFieldBPClass(TEXT("/Script/Engine.Blueprint'/Game/HJS/Blueprints/Actors/FS_HJSBombField_Prototype.FS_HJSBombField_Prototype'"));
	if (MasterFieldBPClass.Succeeded())
	{
		MasterFieldClass = MasterFieldBPClass.Class;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> TempMat(TEXT("/Script/Engine.Material'/Game/HJS/Materials/DecalMaterial.DecalMaterial'"));
	if (TempMat.Succeeded())
	{
		DecalMaterial = TempMat.Object;
	}

	BoomSys = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BoomSys"));
	BoomSys->SetupAttachment(RootComponent);
	BoomSys->bAutoActivate = false;
	BoomSys->SetActive(false);

}

// Called when the game starts or when spawned
void AAIProjecttile_1::BeginPlay()
{
	Super::BeginPlay();
	ProjecttileComp->OnComponentHit.AddDynamic(this, &AAIProjecttile_1::OnHit);
}

// Called every frame
void AAIProjecttile_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIProjecttile_1::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BoomSys)
	{
		BoomSys->Activate();
		BoomSys->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	if (Cast<APawn>(OtherActor) == nullptr) {
		AddDecalAtLocation(Hit.ImpactPoint, Hit.ImpactNormal);
	}

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
	}

	// Player일때
	ATankVehicle* PlayerTank = Cast<ATankVehicle>(OtherActor);
	if (PlayerTank != nullptr)
	{
		PlayerTank->SetPlayerTankDamage(50.0);
	}

	AActor* MyOwner = GetOwner();

	// SunnyEnemy일때
	if (MyOwner != nullptr)
	{
		AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
		if (MyOwnerInstigator != nullptr)
		{
			UClass* DamageTypeClass = UDamageType::StaticClass();
			ASunnyNewTTank* SunnyEnemy = Cast<ASunnyNewTTank>(OtherActor);
			if (SunnyEnemy != nullptr)
			{
				UGameplayStatics::ApplyDamage(OtherActor, 50.f, MyOwnerInstigator, this, DamageTypeClass);
			}
		}
	}

	ProjecttileComp->SetVisibility(false);
	this->SetActorEnableCollision(false);
	GetWorldTimerManager().SetTimer(BulletTimerHandle,this,&AAIProjecttile_1::BulletDestroy,5.f,false);
	return;
}

void AAIProjecttile_1::AddDecalAtLocation(FVector Location, FVector Normal)
{
	// 데칼 생성에 필요한 파라미터 설정
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
		this,
		DecalMaterial, // 대포 자국에 사용할 마테리얼
		FVector(10.0f, 50.0f, 50.0f), // 데칼 크기
		Location,
		Normal.Rotation()
	);

	// 데칼의 수명 설정 (필요에 따라 설정)
	if (Decal)
	{
		Decal->SetLifeSpan(10.0f); // 예: 10초 후에 사라지도록 설정
	}
}

void AAIProjecttile_1::BulletDestroy()
{
	Destroy();
}
