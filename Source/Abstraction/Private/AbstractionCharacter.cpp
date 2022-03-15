// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractionCharacter.h"
#include <AbstractionPlayerController.h>
#include <HealthComponent.h>

// Sets default values
AAbstractionCharacter::AAbstractionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (HealthComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating Health Component"));
		HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	}
}

// Called when the game starts or when spawned
void AAbstractionCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AAbstractionCharacter::BeginPlay: HealthComponent Current Health: %f"), HealthComponent->GetCurrentHealth());
}

// Called every frame
void AAbstractionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAbstractionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAbstractionCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	OnDeath(true);
}

float AAbstractionCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("Prior to Health Component Check"));
	if (HealthComponent != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Taken Damage, %f; With current Health of: %f"), DamageAmount, HealthComponent->GetCurrentHealth());
		HealthComponent->TakeDamage(Damage);
		if (HealthComponent->IsDead())
		{
			OnDeath(false);
		}
	}
	return Damage;
}

void AAbstractionCharacter::OnDeath(bool IsFellOut)
{
	AAbstractionPlayerController* AbstractionPlayerController = GetController<AAbstractionPlayerController>();
	if (AbstractionPlayerController)
	{
		AbstractionPlayerController->RestartLevel();
	}
}
