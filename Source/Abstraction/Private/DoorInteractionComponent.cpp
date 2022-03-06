// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include <GameFramework/Actor.h>
#include <GameFramework/PlayerController.h>
#include <Engine/TriggerBox.h>
#include <Engine/World.h>

#include <DrawDebugHelpers.h>

constexpr float FLT_METERS(float meters) { return meters * 100.0f; }

static TAutoConsoleVariable<bool> CVarToggleDebugDoor(
	TEXT("Abstraction.DoorInteractionComponent.Debug"),
	false,
	TEXT("Toggle DoorInteractionComponent debug display"),
	ECVF_Default);

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CVarToggleDebugDoor.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UDoorInteractionComponent::OnDebugToggled));
}

// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Assumed door is shut at creation
	DoorState = EDoorState::DS_Closed_Closing;
	ClosedDoor = GetOwner()->GetActorRotation();
	ForwardEndRotation = ClosedDoor + DesiredRotation;
	ForwardEndRotation.Normalize();
	BackwardEndRotation = ClosedDoor - DesiredRotation;
	BackwardEndRotation.Normalize();
	ActorFacingAngleOffset = GetOwner()->GetActorForwardVector().HeadingAngle();

	if (Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClosedDoor: %s"), *ClosedDoor.ToString());
		UE_LOG(LogTemp, Warning, TEXT("ForwardEndRotation: %s"), *ForwardEndRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("BackwardEndRotation: %s"), *BackwardEndRotation.ToString());
		UE_LOG(LogTemp, Warning, TEXT("StartFacingAngle: %f"), FMath::RadiansToDegrees(ActorFacingAngleOffset));
	}
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
				if (Debug)
				{
					UE_LOG(LogTemp, Warning, TEXT("FromActorToPawnAngle: %f"), FMath::RadiansToDegrees(FromActorToPawnAngle));
				}

				bool OpenForward = false;
				// The Direction will be opposite the pawn when approaching the door.
				if (FromActorToPawnAngle > 0.0f)
				{
					OpenForward = true;
				}
				DoorDirectionCheck = false;
				DetermineStartEndRotation(OpenForward);
			}
			RotateDoor(DeltaTime);
		}
		else if(!DoorDirectionCheck)
		{
			DoorDirectionCheck = true;
		}
	}
	DebugDraw();
}

void UDoorInteractionComponent::OnDebugToggled(IConsoleVariable* var)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDebugToggled"));
}

float UDoorInteractionComponent::LocalAngleToPawn(const APawn* PlayerPawn)
{
	const FVector ActorToPawn = PlayerPawn->GetPawnViewLocation() - GetOwner()->GetActorLocation();
	// Consider StartFacingAngle 0 Deg
	float ActorToPawnAngle = ActorToPawn.HeadingAngle() - ActorFacingAngleOffset;
	// 1 * Sin(Angle) = Y
	// 1 * Cos(Angle) = X
	// Atan2(Y, X) = [-PI <= 0 <= PI]
	return FMath::Atan2(FMath::Sin(ActorToPawnAngle), FMath::Cos(ActorToPawnAngle));
}

void UDoorInteractionComponent::DetermineStartEndRotation(const bool OpenForward)
{
	StartRotation = GetOwner()->GetActorRotation();

	if (Debug)
	{
		FString Direction = OpenForward ? FString("Door Forward") : FString("Door Backward");
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Direction);
		UE_LOG(LogTemp, Warning, TEXT("Current Rotation: %s"), *StartRotation.ToString());
	}
	FRotator DeltaDoorClosed = StartRotation - ClosedDoor;
	DeltaDoorClosed.Normalize();
	if (OpenForward)
	{
		FRotator DeltaForwardOpenDoor = StartRotation - ForwardEndRotation;
		DeltaForwardOpenDoor.Normalize();
		if (Debug)
		{
			UE_LOG(LogTemp, Warning, TEXT("DeltaDoorClosed: %f; DeltaForwardOpenDoor %f"), DeltaDoorClosed.Yaw, DeltaForwardOpenDoor.Yaw);
		}

		if (FMath::Abs(DeltaDoorClosed.Yaw) < FMath::Abs(DeltaForwardOpenDoor.Yaw) && !StartRotation.Equals(ClosedDoor, 5.0f) || StartRotation.Equals(ForwardEndRotation, 5.0f))
		{
			DoorState = EDoorState::DS_Closed_Closing;
			EndRotation = ClosedDoor;
		}
		else
		{
			DoorState = EDoorState::DS_Open_Opening_Forward;
			EndRotation = ForwardEndRotation;
		}
	}
	else
	{
		FRotator DeltaBackwardOpenDoor = StartRotation - BackwardEndRotation;
		DeltaBackwardOpenDoor.Normalize();
		if (Debug)
		{
			UE_LOG(LogTemp, Warning, TEXT("DeltaDoorClosed: %f; DeltaBackwardOpenDoor %f"), DeltaDoorClosed.Yaw, DeltaBackwardOpenDoor.Yaw);
		}

		if (FMath::Abs(DeltaDoorClosed.Yaw) < FMath::Abs(DeltaBackwardOpenDoor.Yaw) && !StartRotation.Equals(ClosedDoor, 5.0f) || StartRotation.Equals(BackwardEndRotation, 5.0f))
		{
			DoorState = EDoorState::DS_Closed_Closing;
			EndRotation = ClosedDoor;
		}
		else
		{
			DoorState = EDoorState::DS_Open_Opening_Backward;
			EndRotation = BackwardEndRotation;
		}
	}
	CurrentRotationTime = 0.0f;

	if (Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartRotation: %s; EndRotation %s"), *StartRotation.ToString(), *EndRotation.ToString());
	}
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

void UDoorInteractionComponent::DebugDraw()
{
	if (CVarToggleDebugDoor->GetBool())
	{
		FVector Offset(0.0f, 0.0f, FLT_METERS(2.5f));
		FString EnumAsString = TEXT("Door State: ") + UEnum::GetDisplayValueAsText(DoorState).ToString();
		DrawDebugString(GetWorld(), Offset, EnumAsString, GetOwner(), FColor::Blue, 0.0f);
	}
}