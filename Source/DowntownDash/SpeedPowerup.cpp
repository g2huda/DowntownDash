// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeedPowerup.h"


// Sets default values
ASpeedPowerup::ASpeedPowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Percentage = 50.f;

}

// Called when the game starts or when spawned
void ASpeedPowerup::BeginPlay()
{
	Super::BeginPlay();
	isUsed = false;
	
}

// Called every frame
void ASpeedPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpeedPowerup::UsePowerup_Implementation(AMyCustomCharacter * player)
{
	if (!isUsed)
	{
		player->IncreaseSpeed(Percentage, 0);
		isUsed = true;
	}
}
