// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include <AbstractionPlayerController.h>
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

	InitializeDoor();
	AddTriggerBoxCallbacks();
	AddPlayerControllerCallbacks();
}

// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (InteractionState == EInteractionState::IS_Valid)
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			float FromActorToPawnAngle = LocalAngleToPawn(PlayerPawn).mTheta;
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
			DetermineDoorState(RotateDoor(DeltaTime));
		}
	}
	else if (!DoorDirectionCheck)
	{
		DoorDirectionCheck = true;
	}
	DebugDraw();
}

void UDoorInteractionComponent::OnDebugToggled(IConsoleVariable* var)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDebugToggled"));
}

void UDoorInteractionComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetWorld())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (OverlappedActor && OtherActor != TriggerBox && PlayerPawn && OtherActor == PlayerPawn)
		{
			TriggerState = ETriggerState::TS_Inside;
			// Add widget
			FLocalCoor RelativeCoor = LocalAngleToPawn(PlayerPawn);
			float OffsetY, OffsetX;
			RelativeCoor.mYDelta > 0 ? OffsetY = FLT_METERS(0.2f) : OffsetY = FLT_METERS(-0.2f);
			RelativeCoor.mXDelta > 0 ? OffsetX = FLT_METERS(0.2f) : OffsetX = FLT_METERS(-0.2f);
			FVector Offset(OffsetX, OffsetY, FLT_METERS(0.8f));
			DrawDebugString(GetWorld(), Offset, TEXT("Interact"), GetOwner(), FColor::Blue, 1.5f);
		}
	}
}

void UDoorInteractionComponent::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (GetWorld())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (OverlappedActor && OtherActor != TriggerBox && PlayerPawn && OtherActor == PlayerPawn)
		{
			InteractionState = EInteractionState::IS_Invalid;
			TriggerState = ETriggerState::TS_Outside;
			// Remove widget if exists
		}
	}
}

/* PRIVATE FUNCTIONS */
void UDoorInteractionComponent::InitializeDoor()
{
	// Assumed door is shut at creation
	InteractionState = EInteractionState::IS_Invalid;
	TriggerState = ETriggerState::TS_Outside;;
	DoorState = EDoorState::DS_Closed;
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

void UDoorInteractionComponent::AddTriggerBoxCallbacks()
{
	if (TriggerBox)
	{
		TriggerBox->OnActorBeginOverlap.AddDynamic(this, &UDoorInteractionComponent::OnBeginOverlap);
		TriggerBox->OnActorEndOverlap.AddDynamic(this, &UDoorInteractionComponent::OnEndOverlap);
	}
}

void UDoorInteractionComponent::AddPlayerControllerCallbacks()
{
	if (GetWorld())
	{
		AAbstractionPlayerController* APC = dynamic_cast<AAbstractionPlayerController*>(GetWorld()->GetFirstPlayerController());
		if (APC)
		{
			APC->InteractEvent.AddUObject(this, &UDoorInteractionComponent::OnInteract);
		}
	}
}

void UDoorInteractionComponent::OnInteract()
{
	if (TriggerState == ETriggerState::TS_Inside)
	{
		switch (InteractionState)
		{
		case EInteractionState::IS_Invalid:
			InteractionState = EInteractionState::IS_Valid;
			break;
		case EInteractionState::IS_Valid:
			InteractionState = EInteractionState::IS_Invalid;
			break;
		default:
			InteractionState = EInteractionState::IS_Invalid;
		}
		FString EnumAsString = TEXT("Interaction State: ") + UEnum::GetDisplayValueAsText(InteractionState).ToString();
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, EnumAsString);
	}
}

FLocalCoor UDoorInteractionComponent::LocalAngleToPawn(const APawn* PlayerPawn)
{
	const FVector ActorToPawn = PlayerPawn->GetPawnViewLocation() - GetOwner()->GetActorLocation();
	// Consider StartFacingAngle 0 Deg
	float ActorToPawnAngle = ActorToPawn.HeadingAngle() - ActorFacingAngleOffset;
	// 1 * Sin(Angle) = Y
	float Y = FMath::Sin(ActorToPawnAngle);
	// 1 * Cos(Angle) = X
	float X = FMath::Cos(ActorToPawnAngle);
	// Atan2(Y, X) = [-PI <= 0 <= PI]
	float Theta = FMath::Atan2(Y, X);
	return FLocalCoor(Theta, X, Y);
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
			EndRotation = ClosedDoor;
		}
		else
		{
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
			EndRotation = ClosedDoor;
		}
		else
		{
			EndRotation = BackwardEndRotation;
		}
	}
	CurrentRotationTime = 0.0f;

	if (Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartRotation: %s; EndRotation %s"), *StartRotation.ToString(), *EndRotation.ToString());
	}
}

bool UDoorInteractionComponent::RotateDoor(const float DeltaTime)
{
	bool ActivelyRotatingDoor = false;
	if (CurrentRotationTime < TimeToRotate)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, TimeToRotate);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, EndRotation, RotationAlpha);
		GetOwner()->SetActorRelativeRotation(CurrentRotation);
		ActivelyRotatingDoor = true;
	}
	return ActivelyRotatingDoor;
}

void UDoorInteractionComponent::DetermineDoorState(bool ActivelyRotatingDoor)
{
	if (!ActivelyRotatingDoor)
	{
		InteractionState = EInteractionState::IS_Invalid;
		FRotator CurrentRotation = GetOwner()->GetActorRotation();
		if (CurrentRotation.Equals(ClosedDoor, 5.0f))
		{
			DoorState = EDoorState::DS_Closed;
		}
		else if (CurrentRotation.Equals(ForwardEndRotation, 5.0f))
		{
			DoorState = EDoorState::DS_Open_Forward;
		}
		else
		{
			DoorState = EDoorState::DS_OpenBackward;
		}
	}
	else if (DoorState != EDoorState::DS_Moving)
	{
		DoorState = EDoorState::DS_Moving;
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