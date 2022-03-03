// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include <GameFramework/Actor.h>
#include <GameFramework/PlayerController.h>
#include <Engine/TriggerBox.h>
#include <Engine/World.h>

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	StartFacingAngle = GetOwner()->GetActorForwardVector().HeadingAngle();
	ForwardEndRotation = DesiredRotation;
	BackwardEndRotation = DesiredRotation.GetInverse();
}


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn && TriggerBox->IsOverlappingActor(PlayerPawn))
		{
			float FromActorToPawnAngle = LocalAngleToPawn(PlayerPawn);
			if (DoorDirectionCheck)
			{
				FromActorToPawnAngle > 0 
					? DoorOpenForward = false 
					: DoorOpenForward = true;
				DoorDirectionCheck = false;
				DetermineStartEndRotation(DoorOpenForward);
			}
			RotateDoor(DeltaTime);
		}
		else if(!DoorDirectionCheck)
		{
			DoorDirectionCheck = true;
		}
	}
}


float UDoorInteractionComponent::LocalAngleToPawn(const APawn* PlayerPawn)
{
	const FVector FromActorToPawn = GetOwner()->GetActorLocation() - PlayerPawn->GetPawnViewLocation();
	return FromActorToPawn.HeadingAngle() - StartFacingAngle;
}


void UDoorInteractionComponent::DetermineStartEndRotation(const bool OpenForward)
{
	StartRotation = GetOwner()->GetActorRotation();
	if (OpenForward)
	{
		if (StartRotation.Yaw < 0.0f)
		{
			EndRotation = ClosedDoor;
		}
		else
		{
			EndRotation = ForwardEndRotation;
		}
	}
	else
	{
		if (StartRotation.Yaw > 0 )
		{
			EndRotation = ClosedDoor;
		}
		else
		{
			EndRotation = BackwardEndRotation;
		}
	}
	CurrentRotationTime = 0.0f;
}


void UDoorInteractionComponent::RotateDoor(const float DeltaTime)
{
	if (CurrentRotationTime < TimeToRotate)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, TimeToRotate);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, EndRotation, RotationAlpha);
		GetOwner()->SetActorRelativeRotation(CurrentRotation);
	}
}

