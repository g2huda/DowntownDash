// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "Object.h"
//#include "MyCustomCharacter.h"
#include "Powerups.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UPowerups : public UInterface
{
	//GENERATED_BODY()
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class DOWNTOWNDASH_API IPowerups
{
	//GENERATED_BODY()
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		//class UStaticMeshComponent* PowerUpMesh;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power")
		void UsePowerup(AMyCustomCharacter* player);
	//FORCEINLINE class UStaticMeshComponent* GetPowerUpMesh() const { return PowerUpMesh; }
};
