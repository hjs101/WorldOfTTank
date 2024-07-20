// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SunnyHealth.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WORLDOFTTANK_API USunnyHealth : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USunnyHealth();

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;
	float Health = 0.f;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	class ASunnyNewTTank* SunnyTTank;


	float GetHealth() const { return Health; }
	void SetHealth(float val) { Health = val; }

	float GetMaxHealth() const { return MaxHealth; }
	void SetMaxHealth(float val) { MaxHealth = val; }

	void SettingDamageTaken();
		
};
