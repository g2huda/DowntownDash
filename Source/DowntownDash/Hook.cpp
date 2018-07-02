// Fill out your copyright notice in the Description page of Project Settings.

#include "Hook.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyCustomCharacter.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AHook::AHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set up the hook
	Hook = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook"));
	Hook->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	BoxTrigger->AttachToComponent(Hook, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AHook::BeginPlay()
{
	Super::BeginPlay();

	//update trigger bound based on player direction
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller == NULL)
	{
		return;
	}

	AMyCustomCharacter* player = Cast<AMyCustomCharacter>(controller->GetCharacter());
	if (player == NULL)
	{
		return;
	}

	player->OnPlayerDirectionChanged.AddDynamic(this, &AHook::SetupTriggerBounds);

	//check if player entered/left a trigger
	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AHook::BeginOverlap);
	BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AHook::EndOverlap);
}

void AHook::SetupTriggerBounds(float yDirection)
{
	FVector loc = BoxTrigger->GetRelativeTransform().GetLocation();
	FVector newLoc = FVector(loc.X, FMath::Abs(loc.Y)*yDirection, loc.Z);
	BoxTrigger->SetRelativeLocation(newLoc);
}

void AHook::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AMyCustomCharacter* player = Cast<AMyCustomCharacter>(OtherActor);
	if (player == NULL) return;
	player->OnHookNearby.BindUObject(this, &AHook::GetCurrentLocation);
}

void AHook::EndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AMyCustomCharacter* player = Cast<AMyCustomCharacter>(OtherActor);
	if (player == NULL) return;
	if (player->OnHookNearby.IsBound()) player->OnHookNearby.Unbind();
}

USceneComponent* AHook::GetCurrentLocation()
{
	return GetRootComponent();
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

