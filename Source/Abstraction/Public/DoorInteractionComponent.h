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
enum class EDoorState
{
	DS_Closed_Closing UMETA(DisplayName = "Door Closed/Closing"),
	DS_Open_Opening_Forward UMETA(DisplayName = "Door Open/Opening Forward"),
	DS_Open_Opening_Backward  UMETA(DisplayName = "Door Open/Opening Backward")
};

struct LocalCoor
{
	float mTheta = 0.0f;
	float mXDelta = 0.0f;
	float mYDelta = 0.0f;

	LocalCoor(float Theta, float X, float Y)
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

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:
	// Calulates the angle in +/-PI from actor root location to Pawn eye height.
	// The actor starting position offset is used to ensure the X 0 coordinate line
	// is inline with the rotation of the actor.
	// @param PlayerPawn A pawn pointer that is within the TriggerBox
	LocalCoor LocalAngleToPawn(const APawn* PlayerPawn);

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

	// Rotates the door
	void RotateDoor(const float DeltaTime);

	void DebugDraw();

	float ActorFacingAngleOffset;
	FRotator ForwardEndRotation;
	FRotator BackwardEndRotation;
	FRotator ClosedDoor = FRotator::ZeroRotator;
	bool DoorDirectionCheck = true;
};
