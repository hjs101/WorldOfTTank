// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyHealth.h"
//#include "Sunny/SunnyGameMode.h"
#include "Sunny/SunnyEnemy.h"
#include "Sunny/SunnyEnemyFSM.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
USunnyHealth::USunnyHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 초기 체력 설정
	SetMaxHealth(MaxHealth);
	UE_LOG(LogTemp, Error, TEXT("SetMaxHealth() : %.f"), MaxHealth);
	SetHealth(GetMaxHealth());
	UE_LOG(LogTemp, Error, TEXT("SetHealth() : %.f"), Health);
}


// Called when the game starts
void USunnyHealth::BeginPlay()
{
	Super::BeginPlay();


	// Enemy 컴포넌트 추가
	AiEnemy = Cast<ASunnyEnemy>(GetOwner());

	if (AiEnemy == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AiEnemy is null in USunnyHealth::BeginPlay"));
		return;
	}

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &USunnyHealth::DamageTaken);
}


// Called every frame
void USunnyHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



// 데미지 받았을 때 (1. 체력 깍기  2. 체력이 0이면 죽기
void USunnyHealth::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	SetHealth(GetHealth() - Damage);

	//// 체력 갱신
	if (AiEnemy && !AiEnemy->bDie)
	{
		AiEnemy->SetHealthPercent(GetHealth(), GetMaxHealth());
	}
	//AiEnemy->GetHealthPercent(Health, MaxHealth);


	if (GetHealth() <= 0.f && AiEnemy && !AiEnemy->bDie)
	{
		USunnyEnemyFSM* fsm = Cast<USunnyEnemyFSM>(AiEnemy->Fsm);

		if (fsm)
		{
			fsm->EnemyState = EEnemyState::Die;
			UE_LOG(LogTemp, Warning, TEXT("Setting EnemyState to Die for %s"), *AiEnemy->GetName());
		}

	}
}



