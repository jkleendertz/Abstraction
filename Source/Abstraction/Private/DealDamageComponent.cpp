// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamageComponent.h"
#include <Components/CapsuleComponent.h>
#include <AbstractionCharacter.h>

// Sets default values for this component's properties
UDealDamageComponent::UDealDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	if (TriggerCapsule == nullptr)
	{
		TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	}
}


// Called when the game starts
void UDealDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	AddTriggerBoxCallbacks();	
}

void UDealDamageComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	AAbstractionCharacter* AbstractionCharacter = Cast<AAbstractionCharacter>(OtherActor);
	if (AbstractionCharacter)
	{
		if (Debug)
		{
			UE_LOG(LogTemp, Warning, TEXT("Deal Damage On Overlap Begin"));
		}
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		AbstractionCharacter->TakeDamage(BaseDamage, DamageEvent, nullptr, GetOwner());
	}
}

void UDealDamageComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void UDealDamageComponent::AddTriggerBoxCallbacks()
{
	if (TriggerCapsule)
	{
		TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapBegin);
		TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapEnd);
	}
}