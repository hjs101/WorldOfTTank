// Fill out your copyright notice in the Description page of Project Settings.


#include "Sunny/SunnyHealth.h"
#include "Sunny/SunnyNewTTank.h"
#include "Sunny/SunnyNewFSM.h"

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
	//UE_LOG(LogTemp, Error, TEXT("SetMaxHealth() : %.f"), MaxHealth);
	SetHealth(GetMaxHealth());
	//UE_LOG(LogTemp, Error, TEXT("SetHealth() : %.f"), Health);
}


// Called when the game starts
void USunnyHealth::BeginPlay()
{
	Super::BeginPlay();

	// Enemy 컴포넌트 추가
	SunnyAi = Cast<ASunnyNewTTank>(GetOwner());

	if (SunnyAi == nullptr)
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
	//UE_LOG(LogTemp, Error, TEXT("Get Damage!!!"));

	if (Damage <= 0.f) return;

	SetHealth(GetHealth() - Damage);

	//// 체력 갱신
	if (SunnyAi && !SunnyAi->bDead)
	{
		SunnyAi->SetHealthPercent(GetHealth(), GetMaxHealth());
	}


	if (GetHealth() <= 0.f && SunnyAi && !SunnyAi->bDead)
	{
		USunnyNewFSM* fsm = Cast<USunnyNewFSM>(SunnyAi->FSM);

		if (fsm)
		{
			fsm->SunnyAiState = ESunnyAiState::Die;
			//UE_LOG(LogTemp, Warning, TEXT("Setting EnemyState to Die for %s"), *SunnyAi->GetName());
		}

	}
}



