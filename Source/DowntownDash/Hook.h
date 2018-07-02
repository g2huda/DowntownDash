// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hook.generated.h"

UCLASS()
class DOWNTOWNDASH_API AHook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHook();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Hook;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxTrigger;
	UFUNCTION()
	void SetupTriggerBounds(float yDirection);
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	USceneComponent* GetCurrentLocation();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UStaticMeshComponent* GetHook() const { return Hook; }
	
};
