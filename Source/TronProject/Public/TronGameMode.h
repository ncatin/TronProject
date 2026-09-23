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

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StartTimer();

	void UpdateCountdown();

	int32 ReadyPlayers = 0;
	int32 JoinedPlayers = 0;
	
	FTimerHandle RepeatingHandle;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
