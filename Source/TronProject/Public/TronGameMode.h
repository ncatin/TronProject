// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TronGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TRONPROJECT_API ATronGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
};
