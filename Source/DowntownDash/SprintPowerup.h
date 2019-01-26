// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Powerups.h"
#include "MyCustomCharacter.h"
#include "SprintPowerup.generated.h"

UCLASS()
class DOWNTOWNDASH_API ASprintPowerup : public AActor, public IPowerups
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASprintPowerup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool isUsed;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(Category = "Power", EditAnywhere, BlueprintReadWrite)
		float Duration;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power")
		void UsePowerup(AMyCustomCharacter* player);
	virtual void UsePowerup_Implementation(AMyCustomCharacter* player) override;

	//void OnPowerUsed()

	
	
};
