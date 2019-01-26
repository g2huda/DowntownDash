// Fill out your copyright notice in the Description page of Project Settings.

#include "SprintPowerup.h"


// Sets default values
ASprintPowerup::ASprintPowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Duration = 5.0f;
}

// Called when the game starts or when spawned
void ASprintPowerup::BeginPlay()
{
	Super::BeginPlay();
	isUsed = false;
}

// Called every frame
void ASprintPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASprintPowerup::UsePowerup_Implementation(AMyCustomCharacter * player)
{
	FTimerHandle handler;
	FTimerDelegate timerDelegate;
	timerDelegate.BindUFunction(player, FName("EndSprint"));
	player->BeginSprint();

	GetWorld()->GetTimerManager().SetTimer(handler, timerDelegate, Duration, false);
	isUsed = true;
}

