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

	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_Pawn() override;

	virtual void AcknowledgePossession(APawn* P) override;

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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> UIClass;

	UPROPERTY()
	UUserWidget* UIWidget;

	int32 CountdownIndex = 0;

	UFUNCTION(Server, Reliable)
	void ServerSide_PossesPawn(APawn* InPawn);

	UFUNCTION(NetMulticast, Reliable)
	void GameStateCountdown();

	UFUNCTION()
	void MoveLeft();

	UFUNCTION()
	void MoveUp();

	UFUNCTION()
	void MoveDown();

	UFUNCTION()
	void MoveRight();

	UFUNCTION(Server, Reliable)
	void Server_MoveLeft();

	UFUNCTION(Server, Reliable)
	void Server_MoveUp();

	UFUNCTION(Server, Reliable)
	void Server_MoveDown();

	UFUNCTION(Server, Reliable)
	void Server_MoveRight();

	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(Client, Reliable)
	void PossessPawn(APawn* InPawn);

	UFUNCTION(Server, Reliable)
	void Server_SetSpeed(float fspeed);
	
	UFUNCTION()
	void SetSpeed(float fspeed);

	UPROPERTY(EditAnywhere, Category = "Current Direction")
	EMoveDirection CurrentDirection;

	APlayerPawn* ControlledPawn;

	FTimerHandle RepeatingHandle;


	
};
