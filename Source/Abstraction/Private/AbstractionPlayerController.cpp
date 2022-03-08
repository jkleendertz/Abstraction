// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractionPlayerController.h"

AAbstractionPlayerController::AAbstractionPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void AAbstractionPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	if (InputComponent)
	{
		InputComponent->BindAction("Interact", IE_Pressed, this, &AAbstractionPlayerController::EmitInteract);
	}
}

void AAbstractionPlayerController::EmitInteract()
{
	InteractEvent.Broadcast();
}