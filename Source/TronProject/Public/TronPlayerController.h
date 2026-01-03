// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DirectionEnum.h"
#include "TimerManager.h"
#include "TronPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class APlayerPawn;

UCLASS()
class TRONPROJECT_API ATronPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveLeft;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveRight;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveUp;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveDown;

	UFUNCTION()
	void MoveLeft();

	UFUNCTION()
	void MoveUp();

	UFUNCTION()
	void MoveDown();

	UFUNCTION()
	void MoveRight();

	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, Category = "Current Direction")
	EMoveDirection CurrentDirection;

	APlayerPawn* ControlledPawn;


	
};
