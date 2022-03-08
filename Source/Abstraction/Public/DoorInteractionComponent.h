// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Curves/CurveFloat.h>
#include "DoorInteractionComponent.generated.h"

class ATriggerBox;
class IConsoleVariable;

UENUM()
enum class EInteractionState
{
	IS_Valid UMETA(DisplayName = "Valid"),
	IS_Invalid UMETA(DisplayName = "Invalid")
};

UENUM()
enum class ETriggerState
{
	TS_Inside UMETA(DisplayName = "Inside"),
	TS_Outside UMETA(DisplayName = "Outside")
};

UENUM()
enum class EDoorState
{
	DS_Closed UMETA(DisplayName = "Closed"),
	DS_Open_Forward UMETA(DisplayName = "Open Forward"),
	DS_OpenBackward  UMETA(DisplayName = "OpenBackward"),
	DS_Moving  UMETA(DisplayName = "Moving")
};


struct FLocalCoor
{
	float mTheta = 0.0f;
	float mXDelta = 0.0f;
	float mYDelta = 0.0f;

	FLocalCoor(float Theta, float X, float Y)
	{
		mTheta = Theta;
		mXDelta = X;
		mYDelta = Y;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTION_API UDoorInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorInteractionComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static void OnDebugToggled(IConsoleVariable* var);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FRotator DesiredRotation = FRotator::ZeroRotator;

	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator EndRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere)
	float TimeToRotate = 1.0f;

	float CurrentRotationTime = 0.0f;

	UPROPERTY(EditAnywhere)
	ATriggerBox* TriggerBox;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve OpenCurve;

	UPROPERTY(EditAnywhere)
	bool Debug = false;

	UPROPERTY(BlueprintReadOnly)
	EDoorState DoorState;

	UPROPERTY(BlueprintReadOnly)
	EInteractionState InteractionState;

	UPROPERTY(BlueprintReadOnly)
	ETriggerState TriggerState;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:

	// Sets relevant private variables for door to function per UPROPERTY settings.
	void InitializeDoor();

	void AddTriggerBoxCallbacks();

	void AddPlayerControllerCallbacks();

	void OnInteract();

	// Calulates the angle in +/-PI from actor root location to Pawn eye height.
	// The actor starting position offset is used to ensure the X 0 coordinate line
	// is inline with the rotation of the actor.
	// @param PlayerPawn A pawn pointer that is within the TriggerBox
	FLocalCoor LocalAngleToPawn(const APawn* PlayerPawn);

	// Determines the start and end rotation of the door. This will be the direction
	// to the closeset end state. 
	// 1. Open Door Forward
	// 2. Closed Door
	// 3. Open Door Backward
	// The end rotation on triggering will always be towards the closest end state
	// unless if is within 5 degrees of that state already.
	// @param OpenForward A bool indicating the direction the door will move.
	// The actor's start facing angle is used as the reference of 0 deg
	void DetermineStartEndRotation(const bool OpenForward);

	// Rotates the door and returns true if actively rotating door
	bool RotateDoor(const float DeltaTime);

	void DetermineDoorState(bool ActivelyRotatingDoor);

	void DebugDraw();

	float ActorFacingAngleOffset;
	FRotator ForwardEndRotation;
	FRotator BackwardEndRotation;
	FRotator ClosedDoor = FRotator::ZeroRotator;
	bool DoorDirectionCheck = true;
};
