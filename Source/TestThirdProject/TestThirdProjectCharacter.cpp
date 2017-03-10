// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TestThirdProjectCharacter.h"
#include "TestThirdProject.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "UnrealNetwork.h"
#include "MyProjectile.h"
#include  "TestThirdProjectPlayerController.h"


ATestThirdProjectCharacter::ATestThirdProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

										  // Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{	
		CursorDynamicMaterial =  UMaterialInstanceDynamic::Create(DecalMaterialAsset.Object, NULL);
		if (CursorDynamicMaterial)
		{
			CursorToWorld->SetDecalMaterial(CursorDynamicMaterial);
		}
		
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
//	CursorToWorld->SetHiddenInGame(true);
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicateMovement = true;
	FireRate = 0.5f;
	CursorInterSpeed = 10.f;
}

void ATestThirdProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	//UCrowdFollowingComponent* pCrowdComponent = NewObject<UCrowdFollowingComponent>(this);
	InitPathComponent();
}
void ATestThirdProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (PlayerInputComponent)
	{
		InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATestThirdProjectCharacter::OnSetDestinationPressed);
		InputComponent->BindAction("SetDestination", IE_Released, this, &ATestThirdProjectCharacter::OnSetDestinationReleased);

		InputComponent->BindAction("FireShot", IE_Pressed, this, &ATestThirdProjectCharacter::OnFireShotPressed);
		InputComponent->BindAction("FireShot", IE_Released, this, &ATestThirdProjectCharacter::OnFireShotReleased);
		// support touch devices 
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATestThirdProjectCharacter::MoveToTouchLocation);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATestThirdProjectCharacter::MoveToTouchLocation);
		//
		InputComponent->BindAction("SwitchView", IE_Pressed, this, &ATestThirdProjectCharacter::SwitchView);

	}
}

void ATestThirdProjectCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestThirdProjectCharacter, CursorTransform);
}
void ATestThirdProjectCharacter::SwitchView()
{
	APlayerController* pController = Cast<APlayerController>(GetController());
	if (pController)
	{
		TArray<AActor*> PlayerArray;
		UGameplayStatics::GetAllActorsOfClass(this, ATestThirdProjectCharacter::StaticClass(), PlayerArray);
		for (AActor * It : PlayerArray)
		{
			ATestThirdProjectCharacter* pPlayer = Cast<ATestThirdProjectCharacter>(It);
			if (pPlayer && pPlayer != this)
			{
				pController->SetViewTargetWithBlend(pPlayer, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic, 0.f);
			}
		}
	}
}
void ATestThirdProjectCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PostPossessdBy(NewController);
}
void  ATestThirdProjectCharacter::PostPossessdBy_Implementation(AController* NewController)
{

	if (IsLocallyControlled())
	{
		CursorDynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Green);
	}
	else
	{
		CursorDynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);		
	}
	
	
}

void ATestThirdProjectCharacter::InitPathComponent()
{
	UCrowdFollowingComponent* pCrowdComponent = NewObject<UCrowdFollowingComponent>(this);
	pCrowdComponent->RegisterComponentWithWorld(GetWorld());
	pCrowdComponent->Initialize();
	ATestThirdProjectPlayerController* pController = Cast<ATestThirdProjectPlayerController>(GetController());
	if (pController)
	{
		pController->SetPathFollowingComp(pCrowdComponent);
	}
}


void ATestThirdProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params;
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
	if (bMoveToMouseCursor)
	{
		FVector TargetPoint;

		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{

			TargetPoint = GetCursorToWorld()->GetComponentLocation();

		}
		else
		{
			// Trace to see what is under the mouse cursor		
			APlayerController* pController = Cast<APlayerController>(GetController());
			if (pController)
			{
				FHitResult Hit;
				pController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
				if (Hit.bBlockingHit)
				{
					TargetPoint = Hit.ImpactPoint;
				}
			}

		}
		SetNewMoveDestination(TargetPoint);
	}
	if (Role == ROLE_Authority)
	{
		FireShot(GetActorForwardVector());
	}
	else if (Role  == ROLE_AutonomousProxy)
	{	
		ServerSetCursorTransform(CursorToWorld->GetRelativeTransform());
	}
	else if (Role == ROLE_SimulatedProxy)
	{
		UpdateCursorTransform();
	}
}

void ATestThirdProjectCharacter::UpdateCursorTransform()
{
	FVector Loc = FMath::VInterpTo(CursorToWorld->RelativeLocation, CursorTransform.GetLocation(), GetWorld()->GetDeltaSeconds(), CursorInterSpeed);
	FRotator Rot = FMath::RInterpTo(CursorToWorld->RelativeRotation, CursorTransform.GetRotation().Rotator(), GetWorld()->GetDeltaSeconds(), CursorInterSpeed);
	CursorToWorld->SetRelativeTransform(FTransform(Rot, Loc));
}

bool ATestThirdProjectCharacter::ServerSetCursorTransform_Validate(FTransform InTransform)
{
	return true;
}

void ATestThirdProjectCharacter::ServerSetCursorTransform_Implementation(FTransform InTransform)
{
	CursorTransform = InTransform;
}

void ATestThirdProjectCharacter::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	APlayerController* pController = Cast<APlayerController>(GetController());
	if (pController)
	{
		FHitResult HitResult;
		pController->GetHitResultAtScreenPosition(ScreenSpaceLocation, pController->CurrentClickTraceChannel, true, HitResult);
		if (HitResult.bBlockingHit)
		{
			SetNewMoveDestination(HitResult.ImpactPoint);
		}
	}
	
}

void ATestThirdProjectCharacter::SetNewMoveDestination(const FVector DestLocation)
{

	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	float const Distance = FVector::Dist(DestLocation, GetActorLocation());
	if (NavSys && (Distance > 120.0f))
	{
		AController* pController = GetController();
		if (pController)
		{
			NavSys->SimpleMoveToLocation(pController, DestLocation);
		}
	}
}

void ATestThirdProjectCharacter::OnSetDestinationPressed()
{
	bMoveToMouseCursor = true;
}

void ATestThirdProjectCharacter::OnSetDestinationReleased()
{
	bMoveToMouseCursor = false;
}
void ATestThirdProjectCharacter::OnFireShotPressed()
{
	ServerFireShot(true);
}
void ATestThirdProjectCharacter::OnFireShotReleased()
{
	ServerFireShot(false);
}
bool ATestThirdProjectCharacter::ServerFireShot_Validate(bool IsCanFire)
{
	return true;
}
void ATestThirdProjectCharacter::ServerFireShot_Implementation(bool IsCanFire)
{
	bCanFire = IsCanFire;
	if (!bCanFire)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShotTimerExpired);
	}
}

void ATestThirdProjectCharacter::MulticastMoveToDestanation_Implementation(const FVector DestLocation)
{

}

void ATestThirdProjectCharacter::UpdateCharacterRotation()
{
	if (AController* PC = GetController())
	{
		FVector CursorLoc = CursorToWorld->GetComponentLocation();
		FRotator Rot = FMath::RInterpTo(PC->GetControlRotation(), FRotationMatrix::MakeFromX(CursorLoc - GetActorLocation()).Rotator(), GetWorld()->GetDeltaSeconds(), 10.f);
		PC->SetControlRotation(Rot);	
	}
}
void ATestThirdProjectCharacter::FireShot(FVector FireDirection)
{
	// If we it's ok to fire again
	if (bCanFire == true)
	{
		UpdateCharacterRotation();
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);
			if (ProjectileClass)
			{
				AMyProjectile* pProjectile = GetWorld()->SpawnActorDeferred<AMyProjectile>(ProjectileClass, FTransform(FireRotation, SpawnLocation), this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				pProjectile->GetProjectileMesh()->IgnoreActorWhenMoving(this, true);
				pProjectile->FinishSpawning(FTransform(FireRotation, SpawnLocation));
				GetCapsuleComponent()->IgnoreActorWhenMoving(pProjectile, true);

			}
			bCanFire = false;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ATestThirdProjectCharacter::ShotTimerExpired, FireRate);

		}
	}

}
void ATestThirdProjectCharacter::ShotTimerExpired()
{
	bCanFire = true;
}
void ATestThirdProjectCharacter::OnRep_CursorTransform()
{
	//UpdateCursorTransform();
}
