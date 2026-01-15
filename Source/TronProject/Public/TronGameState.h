// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TronGameState.generated.h"

/**
 * 
 */
UCLASS()
class TRONPROJECT_API ATronGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

public:
	FTimerHandle RepeatingHandle;

	UFUNCTION()
	void StartTimer();

	UPROPERTY(Replicated)
	int32 Countdown = 0;

	UFUNCTION()
	void UpdateCountdown();
};
