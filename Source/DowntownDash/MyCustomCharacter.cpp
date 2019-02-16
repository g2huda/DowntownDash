// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCustomCharacter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CableComponent.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMyCustomCharacter::AMyCustomCharacter()
{
	Rope = CreateDefaultSubobject<UCableComponent>(TEXT("Rope"));
	
	Rope->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_currentDirection = 1.f;
	_speed = 1.f;
	_sprintSpeedMultiplier = 2.5;
	SpeedDecrementation = 0.05f;
	GrappleLength = 100.f;
	AverageRopeLength = 500.f;
	GrappleTag = "Grapple";
	bIsGrappling = false;
	DoubleJumpVelocity = 800.f;
	SwingVelocity = 90.f;
	_originalRopeEnd = Rope->EndLocation;
	SwingDirectionEnum = ESwingDirectionEnum::VE_None;

	//vault
	_bIsVaulting = false;
	_maxDistance = 200.f;
	_heightLimit = 300.f;
}

void AMyCustomCharacter::SetPlayerState(EPlayerState state)
{
	_currentPlayerState = state;
}

EPlayerState AMyCustomCharacter::GetPlayerState()
{
	return _currentPlayerState;
}

void AMyCustomCharacter::IncreaseSpeed(float amount, float duration)
{
	//GetCharacterMovement()->AddForce(FVector(0, 1000 * _speed, 0));
	//float total = (amount / 100.0f) + _speed;
	//_speed = total >= 1.f? 1.f : total;
}

// Called when the game starts or when spawned
void AMyCustomCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMyCustomCharacter::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyCustomCharacter::OnOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMyCustomCharacter::EndOverlap);
	Rope->SetHiddenInGame(true);
	SetPlayerState(EPlayerState::VE_Started);
	_defaultMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void AMyCustomCharacter::TouchStarted()
{
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller == nullptr) return;//todo: throw error
	
	float locationX, locationY;
	bool bIsPressed;
	controller->GetInputTouchState(ETouchIndex::Touch1, locationX, locationY, bIsPressed);
	_touchStartLocation = FVector2D(locationX, locationY);
}

void AMyCustomCharacter::TouchEnded()
{
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller == nullptr) return;
	float locationX, locationY, distance;
	bool bIsPressed;
	controller->GetInputTouchState(ETouchIndex::Touch1, locationX, locationY, bIsPressed);
	distance = locationX - _touchStartLocation.X;
	if (FMath::Abs(distance) > 15)//threshold
	{
		if (distance > 0) MoveRight();
		else MoveLeft();
	}
	else Jump();
}

void AMyCustomCharacter::SwitchDirection()
{
	_currentDirection *= -1;
	OnPlayerDirectionChanged.Broadcast(_currentDirection);
}

// Called every frame
void AMyCustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//CanVault();
	//if (!bIsGrappling) 
		AddMovementInput(FVector(0, -1, 0), _currentDirection*_speed);
		if (GetCharacterMovement()->IsWalking())
		{
			//if (_speed > 0 && _speed - (DeltaTime*SpeedDecrementation) >= 0) ImpactSpeed(-DeltaTime * SpeedDecrementation);
		}

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlayerState"), true);
	//if (!EnumPtr) return FString("Invalid");

	FName name = EnumPtr->GetNameByValue((int64)GetPlayerState()); // for EnumValue == VE_Dance returns "VE_Dance"
	//GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Yellow, name.ToString());
}

// Called to bind functionality to input
void AMyCustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("Run", this, &AMyCustomCharacter::Run);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCustomCharacter::Jump);
	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AMyCustomCharacter::Grapple);
	PlayerInputComponent->BindAction("Grapple", IE_Released, this, &AMyCustomCharacter::BreakFromGrapple);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &AMyCustomCharacter::MoveRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &AMyCustomCharacter::MoveLeft);
	PlayerInputComponent->BindAction("Finger1", IE_Pressed, this, &AMyCustomCharacter::TouchStarted);
	PlayerInputComponent->BindAction("Finger1", IE_Released, this, &AMyCustomCharacter::TouchEnded);

}

//obselete
void AMyCustomCharacter::Run(float Value)
{
	if (Controller != NULL && Value != 0)
	{
		SwitchDirection(Value);
	}
}

void AMyCustomCharacter::BeginSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = _defaultMaxSpeed * _sprintSpeedMultiplier;
}

void AMyCustomCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = _defaultMaxSpeed;
}

void AMyCustomCharacter::MoveRight()
{
	SwitchDirection(1.f);
}

void AMyCustomCharacter::MoveLeft()
{
	SwitchDirection(-1.f);
}

bool AMyCustomCharacter::ShouldSwitchDirection(float diff)
{
	if (IsMovingRight() && diff < -10) return true;
	if (IsMovingLeft() && diff > 10) return true;
	return false;
}

bool AMyCustomCharacter::IsMovingRight()
{
	return _currentDirection == 1;
}

bool AMyCustomCharacter::IsMovingLeft()
{
	return _currentDirection == -1;
}

void AMyCustomCharacter::ImpactSpeed(float value)
{
	_speed += value;
}

void AMyCustomCharacter::SwitchDirection(float direction)
{
	_currentDirection = direction;
	OnPlayerDirectionChanged.Broadcast(_currentDirection);
}

void AMyCustomCharacter::SwitchCamera()
{
	if (!GetCharacterMovement()->IsFalling() && HasActiveCameraComponent())
	{
		//TODO swith camera to the other side
	}
}

void AMyCustomCharacter::Jump()
{
	CanVault();
	Super::Jump();
	
	if (CanJumpOff())
	{
		SetPlayerState(EPlayerState::VE_WallJumping);
		JumpOffWall();
	}
	else
	{
		SetPlayerState(EPlayerState::VE_Jumping);
	}
	
}

void AMyCustomCharacter::JumpOffWall()
{
	SwitchDirection();
	FVector upVel = GetActorUpVector() * GetCharacterMovement()->JumpZVelocity;
	FVector forwardVel = GetActorForwardVector()* - DoubleJumpVelocity;
	LaunchCharacter(upVel+forwardVel, true, true);
}

void AMyCustomCharacter::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	
}

void AMyCustomCharacter::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPowerups* TheInterface = Cast<IPowerups>(OtherActor);
	if (TheInterface)
	{
		OtherActor->Destroy();
	}
}

void AMyCustomCharacter::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	IPowerups* TheInterface = Cast<IPowerups>(OtherActor);
	if (TheInterface)
	{
		TheInterface->Execute_UsePowerup(OtherActor, this);
	}
}

void AMyCustomCharacter::OnGrapple()
{
	float time = GetWorld()->GetTimerManager().GetTimerRate(GrappleHandle);
	SwingFromLocation = GetActorLocation();
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	FVector direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), _CurrentRopeLocation);
	//FVector HookToDownDir = UKismetMathLibrary::GetDirectionUnitVector(_CurrentRopeLocation + FVector::UpVector*-_CurrentRopeLength, _CurrentRopeLocation);
	UpdateSwingVelocity();
	//UpdateDirection();
		
	//FRotator look = UKismetMathLibrary::FindLookAtRotation(impactPoint, GetActorLocation());
	float swingDirection = SwingDirectionEnum == ESwingDirectionEnum::VE_Left ? -1 : 1;//TODO might be none
	float angleRotation = SwingVelocity * swingDirection;
	angleRotation = angleRotation * GetWorld()->GetDeltaSeconds();
	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(_CurrentRopeLocation, GetActorLocation());
	
	FVector newLocation = direction.RotateAngleAxis(angleRotation*AverageRopeLength/_CurrentRopeLength, FVector::ForwardVector) * _CurrentRopeLength;
	//DrawDebugLine(GetWorld(), impactPoint, impactPoint - newLocation, FColor::Black, true, 10.0f, '\000', 8.f);

	FHitResult outHit;
	SetActorLocation(_CurrentRopeLocation - newLocation, true, &outHit);
	float diff = SwingFromLocation.Y - GetActorLocation().Y;
	if (ShouldSwitchDirection(diff)) SwitchDirection();
	//GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Yellow, FString::Printf(TEXT("state: %f"), diff));

	if (outHit.bBlockingHit) BreakFromGrapple();
}

void AMyCustomCharacter::Grapple()
{
	Rope->SetHiddenInGame(false);
	if (GetCharacterMovement()->IsWalking() && bIsGrappling)
	{
		BreakFromGrapple();
		return;
	}

	if (GetCharacterMovement()->IsWalking()) return;
	SetPlayerState(EPlayerState::VE_Swinging);
	if (!bIsGrappling) ShootTheRope();
}

void AMyCustomCharacter::BreakFromGrapple()
{
	SetPlayerState(EPlayerState::VE_Running);//TODO: need to change
	SwingDirectionEnum = ESwingDirectionEnum::VE_None;
	if (!bIsGrappling) return;

	FVector dir = GetActorLocation() - SwingFromLocation;
	float dist = FVector::Distance(GetActorLocation(), SwingFromLocation);
	
	LaunchCharacter(dir*dist, true, true);
	//FRotator rotator = UKismetMathLibrary::FindLookAtRotation(_CurrentRopeLocation, GetActorLocation());
	//bool isMovingForward =  rotator.Yaw * GetActorForwardVector().Y >= 0.f;
	//bool isMovingUp = rotator.Pitch < 25.f;
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,   .ToString());
	
	GetWorld()->GetTimerManager().ClearTimer(GrappleHandle);
	bIsGrappling = false;
	ResetRope();
}

void AMyCustomCharacter::ResetRope()
{
	Rope->SetAbsolute(false);
	Rope->RelativeLocation = FVector(0, 0, 0);
	Rope->EndLocation = FVector(0, 0, 0);
	Rope->SetHiddenInGame(true);
}

USceneComponent * AMyCustomCharacter::GetHookComponent()
{
	if (OnHookNearby.IsBound())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("Should return location "));
		return OnHookNearby.Execute();
	}
	return nullptr;
}

void AMyCustomCharacter::ShootTheRope()
{
	SwingDirectionEnum = _currentDirection > 0 ? ESwingDirectionEnum::VE_Left : ESwingDirectionEnum::VE_Right;
	FVector actorLoc = GetActorLocation();
	UCapsuleComponent* capsule = GetCapsuleComponent();
	FVector startPoint = actorLoc + (GetActorUpVector() * capsule->GetUnscaledCapsuleHalfHeight());

	USceneComponent* hookComponent = GetHookComponent();
	if (hookComponent == nullptr) return;
		
	FVector hookLocation = hookComponent->GetComponentLocation();
	//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, startPoint.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, hookLocation.ToString());

	FVector endPoint = FVector(actorLoc.X, hookLocation.Y, hookLocation.Z);

	//setup the hook and start grappling
	if (GetMesh()->DoesSocketExist("GrapPointR"))
	{
		Rope->SetAbsolute(true);
		Rope->SetWorldLocation(endPoint);
		Rope->EndLocation = GetMesh()->GetSocketTransform("GrapPointR", RTS_Actor).GetLocation();
		Rope->CableLength = FVector::Distance(startPoint, endPoint);
		bIsGrappling = true;
		_CurrentRopeLocation = endPoint;
		_CurrentRopeLength = FVector::Distance(startPoint, endPoint);
		GetWorld()->GetTimerManager().SetTimer(GrappleHandle, this, &AMyCustomCharacter::OnGrapple, GetWorld()->GetDeltaSeconds(), true);
	}
	
#pragma region tmp
	//FVector cornerVector = (GetActorForwardVector()*2.f).RotateAngleAxis(-45.f, GetActorRightVector());
	//FVector endPoint = startPoint + (cornerVector * GrappleLength);
//	FHitResult outHit;
//	FCollisionQueryParams param;
//	param.AddIgnoredActor(this);
//	//TODO: need to have a range for grapping 
//	if (GetWorld()->LineTraceSingleByChannel(outHit, startPoint, endPoint, ECC_Pawn, param))
//	{
//		if (outHit.GetActor() && outHit.GetActor()->ActorHasTag(GrappleTag))
//		{
//			if (GetMesh()->DoesSocketExist("GrapPointR"))
//			{
//				Rope->SetAbsolute(true);
//				Rope->SetWorldLocation(outHit.ImpactPoint);
//				Rope->EndLocation = GetMesh()->GetSocketTransform("GrapPointR", RTS_Actor).GetLocation();
//
////				GrappleDel.BindUFunction(this, FName("OnGrapple"), outHit.ImpactPoint, GetActorLocation(), outHit.Distance);
////				GetWorld()->GetTimerManager().SetTimer(GrappleHandle, GrappleDel, 0.01f, true);
//				//GetWorld()->GetTimerManager().SetTimer(GrappleHandle, this, &AMyCustomCharacter::OnGrapple, 0.1f, true);
//			}
//
//			Rope->CableLength = outHit.Distance;// +50;
//			bIsGrappling = true;
//			_CurrentRopeLocation = outHit.ImpactPoint;
//			_CurrentRopeLength = outHit.Distance;
//			GetWorld()->GetTimerManager().SetTimer(GrappleHandle, this, &AMyCustomCharacter::OnGrapple, GetWorld()->GetDeltaSeconds(), true);
//		}
//	}
#pragma endregion
}

void AMyCustomCharacter::UpdateSwingVelocity()
{
	FVector toRopeDir = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), _CurrentRopeLocation);
	FVector curVel = GetVelocity();
	FVector newVel = UKismetMathLibrary::ProjectVectorOnToVector(curVel, toRopeDir);
	GetCharacterMovement()->Velocity.Set(newVel.X, newVel.Y, newVel.Z);
}

bool AMyCustomCharacter::CanVault()
{
	if (_bIsVaulting)
	{
		_bIsVaulting = false;
		return false;
	}

	FHitResult outHit;
	UCapsuleComponent* capsule = GetCapsuleComponent();
	FVector startPoint = GetActorLocation()
		+ (GetActorForwardVector()* capsule->GetUnscaledCapsuleRadius())
		- (GetActorUpVector() * (capsule->GetScaledCapsuleHalfHeight() / 2));
	FVector endPoint = startPoint + (GetActorForwardVector()*_maxDistance);
	FCollisionQueryParams param;
	param.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), startPoint, endPoint, FColor::Red);
	if (GetWorld()->LineTraceSingleByChannel(outHit, startPoint, endPoint, ECC_Pawn, param))
	{
		//todo: check outhit bounds and vault if hight is half character or less and width is sertain bounds
		float distance = outHit.Distance;
		AActor *actor = outHit.GetActor();
		float rootBoneHeight = GetMesh()->GetSocketLocation("RootSocket").Z;//need to check if exists
		float height = actor->GetComponentsBoundingBox().Max.Z - rootBoneHeight;
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, FString::Printf(TEXT("should vault: %f"), height));

		if (height <= _heightLimit)
		{
			GetCharacterMovement()->JumpZVelocity = FMath::Ceil(height * 6);
			_bIsVaulting = true;
			return true;
		}
	}

	GetCharacterMovement()->JumpZVelocity = 1000;//todo: need to set the default
	_bIsVaulting = false;
	return false;
}

bool AMyCustomCharacter::VaultReset()
{
	_bIsVaulting = false;
	return true;
}

bool AMyCustomCharacter::CanJumpOff()
{
	bool isTouchingWall = false;
	bool isInAir = GetCharacterMovement()->IsFalling();
	UCapsuleComponent* capsule = GetCapsuleComponent();
	FVector startPoint = GetActorLocation() + (GetActorForwardVector()* capsule->GetUnscaledCapsuleRadius());
	FVector endPoint = startPoint + (GetActorForwardVector()*1.5);
	FHitResult outHit;
	FCollisionQueryParams param;
	param.AddIgnoredActor(this);
	
	if (GetWorld()->LineTraceSingleByChannel(outHit, startPoint, endPoint, ECC_Pawn, param))
	{
		
		if  (outHit.GetActor()->ActorHasTag("Wall"))
		{
			isTouchingWall = true;
		}
		else
		{
			isTouchingWall = false;
		}
	}
	//DrawDebugLine(GetWorld(), outHit.TraceStart, outHit.TraceEnd, FColor::Red);
	
	return isInAir && isTouchingWall;
}
