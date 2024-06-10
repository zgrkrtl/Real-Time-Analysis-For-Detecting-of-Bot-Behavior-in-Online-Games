// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Tank.generated.h"
/**
 *
 */
UCLASS()
class TOONTANKS_API ATank : public ABasePawn
{
	GENERATED_BODY()
public:
	ATank();
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent *SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent *Camera;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 700.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurnRate = 120.f;

	void Move(float Value);
	void Turn(float Value);

	APlayerController *PlayerControllerRef;

	void LogMovementData();
	void DetectAnomaly();
	void ResumeGame();
	void ClearMovementLog();

	// Log yaw values to file
	FTimerHandle MovementLogTimerHandle;
	float MovementLogInterval = 0.5f;
};
