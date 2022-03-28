// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include <Blueprint/UserWidget.h>
#include <GameFramework/GameModeBase.h>
#include "ObjectiveComponent.h"
#include "ObjectiveWidget.h"
#include "ObjectiveWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ABSTRACTION_API UObjectiveWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void CreateObjectiveWidget(TSubclassOf<UObjectiveWidget> ObjectiveWidgetClass);
	void DisplayObjectiveWidget();

	void OnObjectiveCompleted();

	UFUNCTION(BlueprintCallable)
	FString GetCurrentObjectiveDescription();

	UFUNCTION(BlueprintCallable)
	UObjectiveComponent* GetCurrentObjective();

	UFUNCTION(BlueprintCallable)
	void AddObjective(UObjectiveComponent* ObjectiveComponent);

	UFUNCTION(BlueprintCallable)
	void RemoveObjective(UObjectiveComponent* ObjectiveComponent);

	void OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState);

private:
	UObjectiveWidget* ObjectiveWidget = nullptr;

	TArray<UObjectiveComponent*> Objectives;

	int ObjectiveIndex = 0;
};
