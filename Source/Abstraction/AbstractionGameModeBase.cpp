// Copyright Epic Games, Inc. All Rights Reserved.


#include "AbstractionGameModeBase.h"
#include <Kismet/GameplayStatics.h>

void AAbstractionGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (ObjectiveWidget == nullptr)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ObjectiveWidget = CreateWidget<UUserWidget>(PlayerController, ObjectiveWidgetClass);
	}

	if (ObjectiveWidget)
	{
		ObjectiveWidget->AddToViewport();
	}
}