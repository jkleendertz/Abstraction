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
	// Assumed door is shut at creation
	ClosedDoor = GetOwner()->GetActorRotation();
	ForwardEndRotation = ClosedDoor + DesiredRotation;
	ForwardEndRotation.Normalize();
	BackwardEndRotation = ClosedDoor - DesiredRotation;
	BackwardEndRotation.Normalize();
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
				UE_LOG(LogTemp, Warning, TEXT("FromActorToPawnAngle: %f"), FMath::RadiansToDegrees(FromActorToPawnAngle));
				FromActorToPawnAngle < StartFacingAngle
					? DoorOpenForward = true 
					: DoorOpenForward = false;
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
	return FromActorToPawn.HeadingAngle();
}


void UDoorInteractionComponent::DetermineStartEndRotation(const bool OpenForward)
{
	StartRotation = GetOwner()->GetActorRotation();
	UE_LOG(LogTemp, Warning, TEXT("Current Rotation: %s"), *StartRotation.ToString());
	float DeltaDoorClosed = FMath::Abs(StartRotation.Yaw - ClosedDoor.Yaw);
	if (OpenForward)
	{
		float DeltaForwardOpenDoor = FMath::Abs(StartRotation.Yaw - ForwardEndRotation.Yaw);
		UE_LOG(LogTemp, Warning, TEXT("DeltaDoorClosed: %f; DeltaForwardOpenDoor %f"), DeltaDoorClosed, DeltaForwardOpenDoor);
		if (DeltaDoorClosed > DeltaForwardOpenDoor || StartRotation.Equals(BackwardEndRotation, 5.0f))
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
		float DeltaBackwardOpenDoor = FMath::Abs(StartRotation.Yaw - BackwardEndRotation.Yaw);
		UE_LOG(LogTemp, Warning, TEXT("DeltaDoorClosed: %f; DeltaBackwardOpenDoor %f"), DeltaDoorClosed, DeltaBackwardOpenDoor);
		if (DeltaDoorClosed > DeltaBackwardOpenDoor || StartRotation.Equals(ForwardEndRotation, 5.0f))
		{
			EndRotation = ClosedDoor;
		}
		else
		{
			EndRotation = BackwardEndRotation;
		}
	}
	CurrentRotationTime = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("StartRotation: %s; EndRotation %s"), *StartRotation.ToString(), *EndRotation.ToString());
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