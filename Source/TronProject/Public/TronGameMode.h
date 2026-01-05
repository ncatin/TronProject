// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TronGameMode.generated.h"


class TronPlayerController;
class AStaticCameraActor;

UCLASS()
class TRONPROJECT_API ATronGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> PlayerClass;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> PlayerStarts;

	void BeginPlay() override;

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
};
