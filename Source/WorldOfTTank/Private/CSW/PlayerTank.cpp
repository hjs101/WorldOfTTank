// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/PlayerTank.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerTank::APlayerTank()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 1000;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->FieldOfView = 80;
	
	// ChasingAim = CreateDefaultSubobject<UWidgetComponent>(TEXT("ChasingAim"));
	// ChasingAim->SetupAttachment(SpringArmComp);
}


void APlayerTank::BeginPlay()
{
	Super::BeginPlay();

	// ChangeToTps();
	// ControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}


void APlayerTank::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CamDist[CamIdx] != SpringArmComp->TargetArmLength)
		LerpZoom(DeltaSeconds);
	ChasingAim->SetWorldLocation(GetCurrentHitPoint());
}

// Called to bind functionality to input
void APlayerTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerTank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerTank::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &APlayerTank::LookRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APlayerTank::LookUpDown);

	PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &APlayerTank::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &APlayerTank::ZoomOut);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APlayerTank::Fire);
	PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &APlayerTank::Brake);
}

void	APlayerTank::LookRightLeft(float Value)
{
	AddControllerYawInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	RotateTurret(CameraComp->GetComponentRotation().Yaw);
}

FVector APlayerTank::GetCursorTarget() const
{
	FVector Start = CameraComp->GetComponentLocation();
	FVector End = CameraComp->GetComponentLocation() + CameraComp->GetComponentRotation().Vector() * 15000000;
	FHitResult	Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start , End, ECollisionChannel::ECC_WorldStatic))
		return Hit.Location;
	return FVector::ZeroVector;
}

void APlayerTank::LookUpDown(float Value)
{
	if ((CameraComp->GetComponentRotation().Pitch >= 30 && Value < 0) ||
		(CameraComp->GetComponentRotation().Pitch <= -50) && Value > 0)
		return ;
	AddControllerPitchInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	FVector Target = GetCursorTarget();
	RotateBarrel(Target);
}

void APlayerTank::LerpZoom(float DeltaSeconds)
{
	float pre = SpringArmComp->TargetArmLength;
	SpringArmComp->TargetArmLength = FMath::FInterpTo(
		SpringArmComp->TargetArmLength,
		CamDist[CamIdx],
		DeltaSeconds,
		10);
	if (pre == SpringArmComp->TargetArmLength)
		SpringArmComp->TargetArmLength = CamDist[CamIdx];
}

void APlayerTank::LerpAim(float DeltaSeconds)
{
	FVector pre = ChasingAim->GetComponentLocation();
	FVector target = GetCurrentHitPoint();
	FVector ans = FMath::VInterpTo(pre,
		target,
		DeltaSeconds,
		10);
	ChasingAim->SetWorldLocation(ans);

}


void APlayerTank::ChangeToFps()
{
	if (CurrentAim.IsValid())
	{
		CurrentAim->RemoveFromParent();
		CurrentAim.Reset();
	}
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), FpsAimClass);
	if (NewWidget)
		(CurrentAim = TStrongObjectPtr<UUserWidget>(NewWidget))->AddToViewport();
}

void APlayerTank::ChangeToTps()
{
	if (CurrentAim.IsValid())
	{
		CurrentAim->RemoveFromParent();
		CurrentAim.Reset();
	}
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), TpsAimClass);
	if (NewWidget)
		(CurrentAim = TStrongObjectPtr<UUserWidget>(NewWidget))->AddToViewport();
}

void APlayerTank::ZoomIn()
{
	if (CamIdx == 0 && CameraComp->FieldOfView == 80)
	{
		CameraComp->FieldOfView /= 2;
		FVector Target = GetCursorTarget();
		SpringArmComp->TargetArmLength = 0;
		SpringArmComp->SetupAttachment(ProjectileSpawnPoint);
		CameraComp->SetWorldRotation((Target - CameraComp->GetComponentLocation()).Rotation());
		ChangeToFps();
	}
	else if (CamIdx == 0 && CameraComp->FieldOfView > 5)
		CameraComp->FieldOfView /= 2;
	else if (CamIdx > 0)
		CamIdx--;
}

void APlayerTank::ZoomOut()
{
	if (CamIdx == 0 && CameraComp->FieldOfView < 40)
		CameraComp->FieldOfView *= 2;
	else if (CamIdx == 0 && CameraComp->FieldOfView == 40)
	{
		CameraComp->FieldOfView *= 2;
		SpringArmComp->SetupAttachment(RootComponent);
		SpringArmComp->TargetArmLength = -200;
		
		FVector Target = GetCursorTarget();
		SpringArmComp->SetRelativeLocation(FVector(0, 0, 200));
		SpringArmComp->SetRelativeRotation((Target - SpringArmComp->GetComponentLocation()).Rotation());
		ChangeToTps();
	}
	else if (CamIdx < 5)
		CamIdx++;
}