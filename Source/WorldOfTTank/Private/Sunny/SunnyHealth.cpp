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
	//SunnyTTank = Cast<ASunnyNewTTank>(GetOwner());
}

// Called every frame
void USunnyHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



// 데미지 설정
void USunnyHealth::SettingDamageTaken()
{
	if (SunnyTTank == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Health :: AiEnemy is null in USunnyHealth::BeginPlay"));
		return;
	}

	SunnyTTank->OnTakeAnyDamage.AddDynamic(this, &USunnyHealth::DamageTaken);
}



// 데미지 받았을 때 (1. 체력 깍기  2. 체력이 0이면 죽기
void USunnyHealth::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser)
{
	//UE_LOG(LogTemp, Error, TEXT("Get Damage!!!"));

	if (Damage <= 0.f) return;

	SetHealth(GetHealth() - Damage);

	//// 체력 갱신
	if (SunnyTTank && !SunnyTTank->bDead)
	{
		SunnyTTank->SetHealthPercent(GetHealth(), GetMaxHealth());
	}

	//UE_LOG(LogTemp, Warning, TEXT("DamageTaken()  --> bDead : %d"), SunnyTTank->bDead);

	if (GetHealth() <= 0.f && SunnyTTank && !SunnyTTank->bDead)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enter to change AiState to Die!!!! "));
		SunnyTTank->ChangeStateDie();
	}
}



