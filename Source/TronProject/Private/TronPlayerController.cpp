// Fill out your copyright notice in the Description page of Project Settings.


#include "TronPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "InputMappingContext.h"
#include "PlayerPawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "StaticCameraActor.h"
#include "TronGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/UserWidget.h"


void ATronPlayerController::BeginPlay() {
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	AStaticCameraActor* StaticCamera = GetWorld()->SpawnActor<AStaticCameraActor>(FVector(0.0f, 0.0f, 3000.0f), FRotator(-90.0f, 0.0f, 0.0f));
	SetViewTarget(StaticCamera);

	ConsoleCommand(TEXT("show splines"));
	
	CurrentDirection = EMoveDirection::ED_Left;
	UE_LOG(LogTemp, Warning, TEXT("Current Direction: %s"), *UEnum::GetValueAsString(CurrentDirection));

	if (IsLocalPlayerController()) {
		UIWidget = CreateWidget<UUserWidget>(this, UIClass);
		UIWidget->AddToViewport();
	}
	
}

void ATronPlayerController::Tick(float DeltaTime){

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
	ControlledPawn->speed = 0;

	ATronGameMode* Gamemode = Cast<ATronGameMode>(UGameplayStatics::GetGameMode(this));
	Gamemode->Ready();

	ControlledPawn->OnPossess();
}

void ATronPlayerController::SetSpeed(float fspeed){
	ControlledPawn->speed = fspeed;
}
