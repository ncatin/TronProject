// Fill out your copyright notice in the Description page of Project Settings.


#include "TronPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "PlayerPawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"



void ATronPlayerController::BeginPlay() {
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	ConsoleCommand(TEXT("show splines"));
	//ConsoleCommand(TEXT("show COLLISION"));
	
	ControlledPawn = Cast<APlayerPawn>(GetPawn());
	if(!ControlledPawn) UE_LOG(LogTemp, Warning, TEXT("No Pawn"));
	
	CurrentDirection = EMoveDirection::ED_Left;
	UE_LOG(LogTemp, Warning, TEXT("Current Direction: %s"), *UEnum::GetValueAsString(CurrentDirection));

}


void ATronPlayerController::MoveLeft(){
	
	if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
		FRotator NewRotation = FRotator(0.0f, -90.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Left;
	}

}

void ATronPlayerController::MoveUp(){

	if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
		FRotator NewRotation = FRotator(0.0f, 0.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Up;
	}

}

void ATronPlayerController::MoveDown(){
	if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
		FRotator NewRotation = FRotator(0.0f, 180.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Down;
	}
}

void ATronPlayerController::MoveRight(){
	if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
		FRotator NewRotation = FRotator(0.0f, 90.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Right;
	}
}

void ATronPlayerController::SetupInputComponent(){
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComponent->BindAction(IA_MoveLeft, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveLeft);
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveUp, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveUp);
		EnhancedInputComponent->BindAction(IA_MoveDown, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveDown);
	}
}

void ATronPlayerController::OnPossess(APawn* InPawn){
	ControlledPawn = Cast<APlayerPawn>(InPawn);
	if (!ControlledPawn) UE_LOG(LogTemp, Warning, TEXT("No Pawn"));

	ControlledPawn->EnableInput(this);
}
