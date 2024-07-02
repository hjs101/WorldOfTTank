// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyHealth.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sunny/SunnyGameMode.h"

// Sets default values for this component's properties
USunnyHealth::USunnyHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void USunnyHealth::BeginPlay()
{
	Super::BeginPlay();

	// 초기 체력 설정
	Health = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &USunnyHealth::DamageTaken);
	TTankGameMode = Cast<ASunnyGameMode>(UGameplayStatics::GetGameMode(this));
	
}


// Called every frame
void USunnyHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 데미지 받았을 때 (1. 체력 깍기  2. 체력이 0이면 죽기
void USunnyHealth::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	if (Health <= 0.f && TTankGameMode)
	{
		TTankGameMode->ActorDied(DamagedActor);
	}
}

