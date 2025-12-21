// Fill out your copyright notice in the Description page of Project Settings.


#include "TronPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

void ATronPlayerController::BeginPlay() {
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	CurrentDirection = EMoveDirection::ED_Left;

}

void ATronPlayerController::MoveLeft(){
	FRotator NewRotation;
	switch (CurrentDirection) {
		case EMoveDirection::ED_Up:
			NewRotation = FRotator(0.0f, 0.0f, -90.0f);
			SetControlRotation(NewRotation);
			break;
		case EMoveDirection::ED_Down:
			NewRotation = FRotator(0.0f, 0.0f, 90.0f);
			SetControlRotation(NewRotation);
			break;
		default:
			break;
	}
	CurrentDirection = EMoveDirection::ED_Left;
}

void ATronPlayerController::MoveUp(){
	FRotator NewRotation;
	switch (CurrentDirection) {
	case EMoveDirection::ED_Right:
		NewRotation = FRotator(0.0f, 0.0f, -90.0f);
		SetControlRotation(NewRotation);
		break;
	case EMoveDirection::ED_Left:
		NewRotation = FRotator(0.0f, 0.0f, 90.0f);
		SetControlRotation(NewRotation);
		break;
	default:
		break;
	}
	CurrentDirection = EMoveDirection::ED_Up;
}

void ATronPlayerController::MoveDown(){
	FRotator NewRotation;
	switch (CurrentDirection) {
	case EMoveDirection::ED_Right:
		NewRotation = FRotator(0.0f, 0.0f, 90.0f);
		SetControlRotation(NewRotation);
		break;
	case EMoveDirection::ED_Left:
		NewRotation = FRotator(0.0f, 0.0f, -90.0f);
		SetControlRotation(NewRotation);
		break;
	default:
		break;
	}
	CurrentDirection = EMoveDirection::ED_Down;
}

void ATronPlayerController::MoveRight(){
	FRotator NewRotation;
	switch (CurrentDirection) {
	case EMoveDirection::ED_Up:
		NewRotation = FRotator(0.0f, 0.0f, 90.0f);
		SetControlRotation(NewRotation);
		break;
	case EMoveDirection::ED_Down:
		NewRotation = FRotator(0.0f, 0.0f, -90.0f);
		SetControlRotation(NewRotation);
		break;
	default:
		break;
	}
	CurrentDirection = EMoveDirection::ED_Right;
}

void ATronPlayerController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComponent->BindAction(IA_MoveLeft, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveLeft);
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveUp, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveUp);
		EnhancedInputComponent->BindAction(IA_MoveDown, ETriggerEvent::Triggered, this, &ATronPlayerController::MoveDown);
	}
}
