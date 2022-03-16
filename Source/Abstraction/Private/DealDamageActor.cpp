// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamageActor.h"
#include "DealDamageComponent.h"

#include <Particles/ParticleSystemComponent.h>
#include <Components/CapsuleComponent.h>

// Sets default values
ADealDamageActor::ADealDamageActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (DealDamageComponent == nullptr)
	{
		DealDamageComponent = CreateDefaultSubobject<UDealDamageComponent>(TEXT("Deal Damage"));
		RootComponent = DealDamageComponent->GetTriggerCapsule();
		check(RootComponent)
	}
	if (ParticleSystemComponent == nullptr)
	{
		ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
		ParticleSystemComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void ADealDamageActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADealDamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
