// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ABSTRACTION_API UObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateObjectivetext(FText text);
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectivesText;
};
