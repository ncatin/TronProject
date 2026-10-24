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
class ATronGameMode;


UCLASS()
class TRONPROJECT_API ATronPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_Pawn() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnUnPossess() override;

	UFUNCTION(Server, Reliable)
	void Server_OnDeath();

	UFUNCTION(Server, Reliable)
	void Server_EndMatch();

	UFUNCTION(Server, Reliable)
	void Server_DeductLife(FName PID);

	UFUNCTION(Client, Reliable)
	void Client_DeductLife(FName PID);

public:

	UPROPERTY()
	ATronGameMode* GameMode;

	UPROPERTY(Replicated)
	int32 lives = 3;

	UPROPERTY()
	bool bDead = false;

	UPROPERTY()
	bool bMatchOver = false;

	UPROPERTY()
	FVector SpawnPoint;

	UPROPERTY()
	FName PlayerID;

	UPROPERTY()
	FRotator SpawnRotation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> PlayerPawnClass;

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

	UFUNCTION()
	void DeductLife(FName PID);

	UFUNCTION(Server, Reliable)
	void ServerSide_PossesPawn(APawn* InPawn);

	UFUNCTION(NetMulticast, Reliable)
	void GameStateCountdown(int CountdownIndex);


	UFUNCTION()
	void MoveLeft();

	UFUNCTION()
	void MoveUp();

	UFUNCTION()
	void MoveDown();

	UFUNCTION()
	void MoveRight();

	UFUNCTION(Server, Unreliable)
	void Server_MoveLeft();

	UFUNCTION(Server, Unreliable)
	void Server_MoveUp();

	UFUNCTION(Server, Unreliable)
	void Server_MoveDown();

	UFUNCTION(Server, Unreliable)
	void Server_MoveRight();

	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(Client, Reliable)
	void PossessPawn(APawn* InPawn);

	UFUNCTION(Server, Reliable)
	void Server_SetSpeed(float fspeed);
	
	UFUNCTION()
	void SetSpeed(float fspeed);

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void EndMatch();

	UPROPERTY(EditAnywhere, Category = "Current Direction")
	EMoveDirection CurrentDirection;

	APlayerPawn* ControlledPawn;

	FTimerHandle RepeatingHandle;


	
};
