// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/PlayerTankVehicle.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "CSW/ChasingAim.h"
#include "GameFramework/SpringArmComponent.h"
#include "HJS/AITankCPU_1.h"
#include "PaperSpriteComponent.h"

APlayerTankVehicle::APlayerTankVehicle()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent, FName("turret_jnt"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 2000;
	SpringArmComp->SetRelativeLocation(FVector(0,0,500));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->FieldOfView = 100;

	FpsSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("FpsSpringArm"));
	FpsSpringArmComp->SetupAttachment(RootComponent, FName("gun_jnt"));
	FpsSpringArmComp->SetRelativeLocation(FVector(0, 0, 0));
	FpsSpringArmComp->TargetArmLength = -500;
	FpsSpringArmComp->bUsePawnControlRotation = true;
	FpsSpringArmComp->TargetArmLength = 0;
	
	FpsCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCamera"));
	FpsCameraComp->SetupAttachment(FpsSpringArmComp);
	FpsCameraComp->FieldOfView = 80;

	ChasingAim = CreateDefaultSubobject<UWidgetComponent>(TEXT("ChasingAim"));
	ChasingAim->SetupAttachment(SpringArmComp);

	// Indicator 컴포넌트 추가
	Indicator = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Indicator"));
	Indicator->SetupAttachment(RootComponent);
}
void APlayerTankVehicle::BeginPlay()
{
	Super::BeginPlay();

	ChangeToTps();
	ControllerRef = Cast<APlayerController>(GetController());
}

void APlayerTankVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FHitResult hit;
	GetCurrentHitPoint(hit);
	AActor* tmp = hit.GetActor();
	if (Cast<AAITankCPU_1>(tmp))
		Cast<UChasingAim>(ChasingAim->GetUserWidgetObject())->SetIsHit(true);
	else
		Cast<UChasingAim>(ChasingAim->GetUserWidgetObject())->SetIsHit(false);
	ChasingAim->SetWorldLocation(hit.ImpactPoint);
	FRotator Cam = FpsCameraComp->GetComponentRotation();
	if (CamDist[CamIdx] != SpringArmComp->TargetArmLength)
		LerpZoom(DeltaSeconds);
}

void APlayerTankVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerTankVehicle::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerTankVehicle::Turn);

	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &APlayerTankVehicle::LookRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APlayerTankVehicle::LookUpDown);

	PlayerInputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &APlayerTankVehicle::ZoomIn);
	PlayerInputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &APlayerTankVehicle::ZoomOut);
	
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APlayerTankVehicle::Fire);
	PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &APlayerTankVehicle::Brake);
}


void APlayerTankVehicle::LookRightLeft(float Value) 
{
	AddControllerYawInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	FVector goal = GetCursorTarget() - GetProjectileSpawnPoint()->GetComponentLocation();
	RotateTurret(goal.Rotation().Yaw - GetMesh()->GetComponentRotation().Yaw);
	
}

FVector APlayerTankVehicle::GetCursorTarget() const
{
	FVector2d viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);
	FVector org, dir;
	float hieghtRate = IsFps ? 0.5f : 0.428f;
	ControllerRef->DeprojectScreenPositionToWorld(viewportSize.X / 2, viewportSize.Y * hieghtRate, org, dir);
	FVector end = org + dir * 1000000000;

	FHitResult hitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, org , end, ECollisionChannel::ECC_WorldStatic, Params))
		return hitResult.Location;
	return FVector::ZeroVector;
}

void APlayerTankVehicle::LookUpDown(float Value)
{
	AddControllerPitchInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());


	FVector goal = GetCursorTarget() - GetProjectileSpawnPoint()->GetComponentLocation();
	RotateBarrel(goal.Rotation().Pitch - GetMesh()->GetBoneTransform(FName("turret_jnt")).Rotator().Pitch);

}

void APlayerTankVehicle::LerpZoom(float DeltaSeconds)
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

void APlayerTankVehicle::ChangeToFps()
{
	CameraComp->Deactivate();
	FRotator look = (GetCursorTarget() - FpsCameraComp->GetComponentLocation()).Rotation();
	FpsCameraComp->SetWorldRotation(look);
	FpsCameraComp->Activate();

	IsFps = true;
	if (CurrentAim.IsValid())
	{
		CurrentAim->RemoveFromParent();
		CurrentAim.Reset();
	}
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), FpsAimClass);
	if (NewWidget)
		(CurrentAim = TStrongObjectPtr<UUserWidget>(NewWidget))->AddToViewport();
}

void APlayerTankVehicle::ChangeToTps()
{
	CameraComp->Activate();
	FpsCameraComp->Deactivate();
	IsFps = false;
	if (CurrentAim.IsValid())
	{
		CurrentAim->RemoveFromParent();
		CurrentAim.Reset();
	}
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), TpsAimClass);
	if (NewWidget)
		(CurrentAim = TStrongObjectPtr<UUserWidget>(NewWidget))->AddToViewport();
}


void APlayerTankVehicle::ZoomIn()
{
	if (IsFps == false && CamIdx == 0)
		ChangeToFps();
	if (IsFps == true && FpsCameraComp->FieldOfView != 5)
	{
		FpsCameraComp->FieldOfView /= 2;
	}
	else if (IsFps == false)
			CamIdx--;
}

void APlayerTankVehicle::ZoomOut()
{
	if (IsFps == true && FpsCameraComp->FieldOfView == 80)
		ChangeToTps();
	if (IsFps == true)
		FpsCameraComp->FieldOfView *= 2;
	else if (IsFps == false && CamIdx != 5)
		CamIdx++;
}