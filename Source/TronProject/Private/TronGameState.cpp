// Fill out your copyright notice in the Description page of Project Settings.


#include "TronGameState.h"
#include "TronPlayerController.h"
#include "Gameframework/PlayerState.h"
#include <Net/UnrealNetwork.h>

void ATronGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATronGameState::BeginPlay(){
	Super::BeginPlay();
	
}


