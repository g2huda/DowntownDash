// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Components/InputComponent.h"
#include "MyCustomCharacter.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESwingDirectionEnum : uint8
{
	VE_Left 	UMETA(DisplayName = "SwingLeft"),
	VE_Right 	UMETA(DisplayName = "SwingRight"),
	VE_None		UMETA(DisplayName = "None")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EPlayerState : uint8
{
	VE_Stopped 	UMETA(DisplayName = "Stopped"),
	VE_Started	UMETA(DisplayName = "Started"),
	VE_Running 	UMETA(DisplayName = "Running"),
	VE_Dodging	UMETA(DisplayName = "Dodging"),
	VE_Swinging	UMETA(DisplayName = "Swinging"),
	VE_Jumping	UMETA(DisplayName = "Jumping"),
	VE_WallJumping	UMETA(DisplayName = "WallJumping"),
	VE_Dead	UMETA(DisplayName = "Dead")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerDirectionChanged, float, yDirection);
DECLARE_DELEGATE_RetVal(USceneComponent* , FHookNearbySignature); 

UCLASS()
class DOWNTOWNDASH_API AMyCustomCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCustomCharacter();

	UFUNCTION()
		void SetPlayerState(EPlayerState state);
	UFUNCTION()
		EPlayerState GetPlayerState();

	UPROPERTY(BlueprintAssignable, Category = "Direction")
		FPlayerDirectionChanged OnPlayerDirectionChanged;
	FHookNearbySignature OnHookNearby;

protected:
	UPROPERTY()
		EPlayerState _currentPlayerState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCableComponent* Rope;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Moving
	void Run(float Value);

	void MoveRight();

	void MoveLeft();

	bool ShouldSwitchDirection(float diff);
	void SwitchDirection();

	bool IsMovingRight();
	bool IsMovingLeft();
	void ImpactSpeed(float value);

#pragma endregion

	void SwitchDirection(float direction);
	UFUNCTION()
		void SwitchCamera();

#pragma region Jumping
	void Jump();

	UFUNCTION()
	void JumpOffWall();

	

	UFUNCTION(BlueprintPure)
	bool CanJumpOff();
#pragma endregion

	UFUNCTION()
	void OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit);

#pragma region Grappling

	
	//UFUNCTION()
		void OnGrapple(/*FVector impactPoint, FVector ActorStartingPoint, float distance*/);

	UFUNCTION()
	void Grapple();

	UFUNCTION()
	void BreakFromGrapple();

	void ResetRope();

	UFUNCTION()
	USceneComponent* GetHookComponent();

	UFUNCTION()
		void ShootTheRope();
		void UpdateSwingVelocity();
#pragma endregion
	
	void Slide(float Value);

#pragma region Fields

	//UPROPERTY()
		float _currentDirection;
		UPROPERTY(Category = "Running", BlueprintReadOnly)
		float _speed;
		UPROPERTY(Category = "Running", EditAnywhere, BlueprintReadWrite)
			float SpeedDecrementation;

	UPROPERTY(Category = "Grappling", EditAnywhere, BlueprintReadWrite)
		float GrappleLength;

	UPROPERTY(Category = "Grappling", EditAnywhere, BlueprintReadWrite)
		FName GrappleTag;

	UPROPERTY(Category = "Grappling", EditAnywhere, BlueprintReadWrite)
		float SwingVelocity;

	//UPROPERTY()
		FTimerHandle GrappleHandle;
	FTimerDelegate GrappleDel;

	//UPROPERTY()
		float AverageRopeLength;
	//UPROPERTY()
		FVector _CurrentRopeLocation;
	//UPROPERTY()
		float _CurrentRopeLength;
	//UPROPERTY()
		FVector SwingFromLocation;
	//UPROPERTY()
		bool bIsGrappling;

	//UPROPERTY()
		FVector _originalRopeLocation;

	//UPROPERTY()
		FVector _originalRopeEnd;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		ESwingDirectionEnum SwingDirectionEnum;

	UPROPERTY(Category = "Grappling", EditAnywhere, BlueprintReadWrite)
		float DoubleJumpVelocity;

#pragma endregion

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UCableComponent* GetRope() const { return Rope; }

	
	
};
