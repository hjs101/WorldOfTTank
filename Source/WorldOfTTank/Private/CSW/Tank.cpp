// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/Tank.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"

// Sets default values
ATank::ATank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collier"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	BarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel Mesh"));
	BarrelMesh->SetupAttachment(TurretMesh);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	PlayerControllerRef = Cast<APlayerController>(GetController());
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerControllerRef)
	{
		FHitResult	HitResult;
		PlayerControllerRef->GetHitResultUnderCursor(
			ECC_Visibility,
			false,
			HitResult);

		DrawDebugCircle(
			GetWorld(),
			HitResult.ImpactPoint,
			20,
			12,
			FColor::White,
			false,
			-1);

		RotateTurretAndBarrel(HitResult.ImpactPoint);
		
		// int32	ViewPortSizeX, ViewPortSizeY;
		// PlayerControllerRef->GetViewportSize(ViewPortSizeX, ViewPortSizeY);
		// PlayerControllerRef->SetMouseLocation(ViewPortSizeX/ 2, ViewPortSizeY / 2);
	}
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &ATank::LookRightLeft);
}

void	ATank::Move(float Value)
{
	FVector	DeltaLocation = FVector::ZeroVector;

	DeltaLocation.X = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * Speed;
	AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(float Value)
{
	FRotator DeltaRotation = FRotator::ZeroRotator;

	DeltaRotation.Yaw = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * TurnRate;
	AddActorLocalRotation(DeltaRotation, true);
}

void ATank::LookRightLeft(float Value)
{
	
}

void ATank::RotateTurretAndBarrel(FVector TargetLocation)
{
	FVector		ToTarget = TargetLocation - TurretMesh->GetComponentLocation();
	
	TurretMesh->SetWorldRotation(
		FMath::RInterpTo(
			TurretMesh->GetComponentRotation(),
			FRotator(0, ToTarget.Rotation().Yaw, 0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			3)
			);

	BarrelMesh->SetWorldRotation(
		FMath::RInterpTo(
			BarrelMesh->GetComponentRotation(),
			FRotator(ToTarget.Rotation().Pitch, TurretMesh->GetComponentRotation().Yaw, 0),
			UGameplayStatics::GetWorldDeltaSeconds(this),
			3)
			);

	// SpringArmComp->AddRelativeRotation(
	// 	FRotator(
	// 		TargetLocation.Rotation().Pitch / 100,
	// 		TargetLocation.Rotation().Yaw / 100,
	// 		TargetLocation.Rotation().Roll / 100)
	// 		);
}


