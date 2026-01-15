// Fill out your copyright notice in the Description page of Project Settings.


#include "TronGameState.h"
#include "TronPlayerController.h"
#include "Gameframework/PlayerState.h"
#include <Net/UnrealNetwork.h>

void ATronGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATronGameState, Countdown);
}

void ATronGameState::BeginPlay(){
	Super::BeginPlay();
	
}

void ATronGameState::StartTimer(){
	UE_LOG(LogTemp, Warning, TEXT("Starting Timer"));
	if (HasAuthority()) {
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(RepeatingHandle, this, &ATronGameState::UpdateCountdown, 1, true);
	}
}

void ATronGameState::UpdateCountdown(){
	UE_LOG(LogTemp, Warning, TEXT("counting down"));
	if (Countdown <= 5) {
		Countdown++;
		for (APlayerState* PlayerState : PlayerArray) {
			ATronPlayerController* PlayerController = Cast<ATronPlayerController>(PlayerState->GetOwner());
			PlayerController->GameStateCountdown();
		}
	}
	else {
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(RepeatingHandle);
	}
	
	
}
