// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SunnyEnemyFSM.generated.h"

// 사용할 상태 정의
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDOFTTANK_API USunnyEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USunnyEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 상태 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=FSM)
	EEnemyState mState = EEnemyState::Idle;

	// 대기 상태
	void IdleState();
	// 이동 상태
	void MoveState();
	// 공격 상태
	void AttackState();
	// 피격 상태
	void DamageState();
	// 죽음 상태
	void DieState();


	// 대기 시간
	UPROPERTY(EditDefaultsOnly, Category=FSM)
	float IdleDelayTime = 2.f;
	// 경과 시간
	float CurrentTime = 0;


	// 타깃 (써니 땅크)
	UPROPERTY(VisibleAnywhere, Category=FSM)
	class ASunnyTTank* Target;

	// 소유 액터
	UPROPERTY()
	class ASunnyEnemy* Me;


	// 공격 범위
	UPROPERTY(EditAnywhere, Category=FSM)
	float AttackRange = 150.0f;

	// 공격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float AttackDelayTime = 2.0f;
};
