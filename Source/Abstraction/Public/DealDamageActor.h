// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DealDamageActor.generated.h"


class UDealDamageComponent;
class UParticleSystemComponent;

UCLASS()
class ABSTRACTION_API ADealDamageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADealDamageActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, NoClear)
	UDealDamageComponent* DealDamageComponent;

	UPROPERTY(EditAnywhere, NoClear)
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(EditAnywhere)
	float ToggleTime = 5.0f;

private:
	float CurrentTimer = 0.0f;
};
