// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TronGameMode.generated.h"


class ATronPlayerController;
class AStaticCameraActor;

UCLASS()
class TRONPROJECT_API ATronGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> PlayerOne;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> PlayerTwo;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> PlayerStarts;

	UPROPERTY(EditAnywhere)
	TArray<ATronPlayerController*> PlayerControllers;

	UPROPERTY(Replicated)
	int32 Countdown = 0;

	void BeginPlay() override;

	UFUNCTION()
	void EndMatch(ATronPlayerController* P);

	UFUNCTION()
	void ResetGame(FName PlayerID);

protected:
	UFUNCTION()
	void SpawnPawns(ATronPlayerController* P);

	UFUNCTION()
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION()
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void StartTimer();

	UFUNCTION()
	void UpdateCountdown();

	
	UPROPERTY()
	bool bEndingMatch = false;

	UPROPERTY()
	int32 ReadyPlayers = 0;

	UPROPERTY()
	int32 JoinedPlayers = 0;
	
	UPROPERTY()
	FTimerHandle RepeatingHandle;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
