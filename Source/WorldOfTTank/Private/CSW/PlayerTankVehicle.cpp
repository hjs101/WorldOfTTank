// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/PlayerTankVehicle.h"

#include "AudioDevice.h"
#include "AudioDeviceManager.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "CSW/ChasingAim.h"
#include "GameFramework/SpringArmComponent.h"
#include "HJS/AITankCPU_1.h"
#include "PaperSpriteComponent.h"
#include "Components/AudioComponent.h"

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
	GetInnerFireSoundComp()->SetSound(InnerFireSound);
	GetOuterFireSoundComp()->SetSound(OuterFireSound);
	GetTrackSoundComp()->SetSound(TrackSound);
}

void APlayerTankVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	if (!IsFps)
	{
		float dist = SpringArmComp->TargetArmLength;
		float volume = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 2000.f), FVector2D(1.f, 0.5f), dist);
		GetOuterFireSoundComp()->SetVolumeMultiplier(volume);
		GetTrackSoundComp()->SetVolumeMultiplier(volume);
		USoundClass* SoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Script/Engine.SoundClass'/Game/CSW/OuterSound.OuterSound'"));
		SoundClass->Properties.Volume = 1.f;
		GetInnerFireSoundComp()->SetVolumeMultiplier(0.f);
	}
	else
	{
		USoundClass* SoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Script/Engine.SoundClass'/Game/CSW/OuterSound.OuterSound'"));
		SoundClass->Properties.Volume = 0.3f;
		GetInnerFireSoundComp()->SetVolumeMultiplier(1.f);
	}
	
	FHitResult hit;
	GetCurrentHitPoint(hit);
	AAITankCPU_1* tmp = Cast<AAITankCPU_1>(hit.GetActor());
	if (tmp)
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

	if (!(GetWorld()->LineTraceSingleByChannel(hitResult, org , end, ECollisionChannel::ECC_WorldStatic, Params)))
		return FVector::ZeroVector;
	static AAITankCPU_1* tmp;
	if (tmp)
		tmp->OffOutLine();
	tmp = Cast<AAITankCPU_1>(hitResult.GetActor());
	if (tmp)
		tmp->OnOutLine();
	return hitResult.Location;
}

void APlayerTankVehicle::LookUpDown(float Value)
{
	// Value = FMath::Clamp(Value, 60, -60);
	if (!IsFps &&
		((CameraComp->GetComponentRotation().Pitch >= 30 && Value < 0) ||
		(CameraComp->GetComponentRotation().Pitch <= -50) && Value > 0))
		return ;
	AddControllerPitchInput(Value * ViewRotationRate * GetWorld()->GetDeltaSeconds());
	
	FVector goal = GetCursorTarget() - GetProjectileSpawnPoint()->GetComponentLocation();
	float newPitch = goal.Rotation().Pitch - GetMesh()->GetBoneTransform(FName("turret_jnt")).Rotator().Pitch;
	newPitch = FMath::Clamp(newPitch, -3.f, 20.f);
	RotateBarrel(newPitch);

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