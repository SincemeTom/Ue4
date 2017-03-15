// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "TestThirdProjectCharacter.generated.h"

UCLASS(Blueprintable)
class ATestThirdProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATestThirdProjectCharacter();

	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	void InitPathComponent();
	UFUNCTION(Client, Reliable)
	virtual void PostPossessdBy(AController* NewController);

	virtual void  PostPossessdBy_Implementation(AController* NewController);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	

	UFUNCTION()
		void OnRep_CursorTransform();
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* CursorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CursorInterSpeed;

	UPROPERTY(replicatedUsing = OnRep_CursorTransform)
	FTransform CursorTransform;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;
	UMaterialInstanceDynamic* CursorDynamicMaterial;

	void UpdateCursorTransform();
	UFUNCTION(Server, unreliable, WithValidation)
	void ServerSetCursorTransform(FTransform InTransform);

	bool ServerSetCursorTransform_Validate(FTransform InTransform);

	void ServerSetCursorTransform_Implementation(FTransform InTransform);
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireShot(bool IsCanFire);

	void ServerFireShot_Implementation(bool IsCanFire);

	bool ServerFireShot_Validate(bool IsCanFire);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastMoveToDestanation(const FVector DestLocation);

	void MulticastMoveToDestanation_Implementation(const FVector DestLocation);

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void OnFireShotPressed();
	void OnFireShotReleased();

	void MoveToDestanation(const FVector DestLocation);

	void UpdateCharacterRotation();

	void FireShot(FVector FireDirection);

	void ShotTimerExpired();

	void SwitchView();

	UFUNCTION(BlueprintCallable, Category = SaveGame)
		void SaveGame(AActor* InActor, const FString& String);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
		bool LoadGame(const FString& SlotName, AActor*& OutActor, FString& String);
private:
	uint8 bCanFire : 1;
	FTimerHandle TimerHandle_ShotTimerExpired;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
		float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
		FVector GunOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
		TSubclassOf<class AMyProjectile> ProjectileClass;
};

