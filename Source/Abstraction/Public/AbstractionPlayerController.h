// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbstractionPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ABSTRACTION_API AAbstractionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAbstractionPlayerController(const FObjectInitializer& ObjectInitializer);

	DECLARE_EVENT(FAbstractionPlayerController, FPlayerInteract)
	FPlayerInteract& OnInteract() { return InteractEvent; }
	FPlayerInteract InteractEvent;

protected:
	virtual void SetupInputComponent() override;

private:
	void EmitInteract();	
};
