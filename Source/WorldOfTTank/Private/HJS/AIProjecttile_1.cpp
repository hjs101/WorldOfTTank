// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFrameWork/ProjectileMovementComponent.h"
#include "HJS/AIProjecttile_1.h"
#include "HJS/FractureWall.h"
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

	
	AFractureWall* FractureComp = Cast<AFractureWall>(OtherActor);

	if (FractureComp != nullptr) {
		
		if (!FractureComp->HpDown()) {
			FTransform HitTransform;
			HitTransform.SetLocation(Hit.Location);
			HitTransform.SetRotation(FRotator(GetActorRotation().Pitch - 90.f, GetActorRotation().Yaw, GetActorRotation().Roll).Quaternion());
			HitTransform.SetScale3D(FVector(1.f, 1.f, 1.f));
			GetWorld()->SpawnActor<AActor>(MasterFieldClass, HitTransform);

			FractureComp->SetDestroyTimer();
		}
	}
	



	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		Destroy();
		return;
	}

	//AController* MyOwnerInstigator = MyOwner->GetInstigatorController();
	//UClass* DamageTypeClass = UDamageType::StaticClass();

	//if (OtherActor != nullptr && OtherActor != this && OtherActor != MyOwner)
	//{
	//	UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
	//	if (HitParticles)
	//	{
	//		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
	//	}

	//	if (HitSound)
	//	{
	//		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	//	}

	//	if (HitCameraShakeClass)
	//	{
	//		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
	//	}
	//}
	Destroy();
	return;
}
