// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveWorldSubsystem.h"
#include <Kismet/GameplayStatics.h>

void UObjectiveWorldSubsystem::CreateObjectiveWidget(TSubclassOf<UObjectiveWidget> ObjectiveWidgetClass)
{
	if (ObjectiveWidget == nullptr)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ObjectiveWidget = CreateWidget<UObjectiveWidget>(PlayerController, ObjectiveWidgetClass);
	}
}

void UObjectiveWorldSubsystem::DisplayObjectiveWidget()
{
	if(ObjectiveWidget)
	{ 
		ObjectiveWidget->UpdateObjectivetext(FText::FromString(GetCurrentObjectiveDescription()));
		if (!ObjectiveWidget->IsInViewport())
		{
			ObjectiveWidget->AddToViewport();
		}
	}
}

void UObjectiveWorldSubsystem::OnObjectiveCompleted()
{
	DisplayObjectiveWidget();
}

FString UObjectiveWorldSubsystem::GetCurrentObjectiveDescription()
{
	if (Objectives.Num() == 0)
	{
		return TEXT("N/A");
	}

	FString RetObjective = Objectives[ObjectiveIndex]->GetDescription();
	UE_LOG(LogTemp, Warning, TEXT("Objectives: %i Index: %i Current Objective Desc: %s"), Objectives.Num(), ObjectiveIndex , *RetObjective);
	if (Objectives[ObjectiveIndex]->GetState() == EObjectiveState::OS_Completed)
	{
		if (Objectives.Num() > ObjectiveIndex)
		{
			ObjectiveIndex++;
			UE_LOG(LogTemp, Warning, TEXT("Objective Incremented 1"));
		}
		return RetObjective += TEXT(" Completed");
	}
	return RetObjective;
}

UObjectiveComponent* UObjectiveWorldSubsystem::GetCurrentObjective()
{
	return Objectives[ObjectiveIndex];
}

void UObjectiveWorldSubsystem::AddObjective(UObjectiveComponent* ObjectiveComponent)
{
	check(ObjectiveComponent);

	size_t PrevSize = Objectives.Num();
	Objectives.AddUnique(ObjectiveComponent);
	if (Objectives.Num() > PrevSize)
	{
		ObjectiveComponent->OnStateChanged().AddUObject(this, &UObjectiveWorldSubsystem::OnObjectiveStateChanged);
	}	
}

void UObjectiveWorldSubsystem::RemoveObjective(UObjectiveComponent* ObjectiveComponent)
{
	Objectives.Remove(ObjectiveComponent);

}

void UObjectiveWorldSubsystem::OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState)
{
	DisplayObjectiveWidget();
}