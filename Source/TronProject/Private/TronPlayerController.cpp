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
#include "Net/UnrealNetwork.h"
#include "GameWidget.h"
#include "TronGameState.h"


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

void ATronPlayerController::OnRep_Pawn()
{
}

void ATronPlayerController::AcknowledgePossession(APawn* P){
	Super::AcknowledgePossession(P);
	ControlledPawn = Cast<APlayerPawn>(P);
	if (!ControlledPawn) UE_LOG(LogTemp, Warning, TEXT("No Pawn"));
	UE_LOG(LogTemp, Warning, TEXT("Possession Acknowledged"));
	ControlledPawn->speed = 0;

	ControlledPawn->OnPossess();
}


void ATronPlayerController::ServerSide_PossesPawn_Implementation(APawn* InPawn){
	if (InPawn && HasAuthority()) {
		Possess(InPawn);
	}
}


void ATronPlayerController::GameStateCountdown_Implementation(){
	CountdownIndex = Cast<ATronGameState>(GetWorld()->GetGameState())->Countdown;
	if (UIWidget) {
		UGameWidget* GameWidget = Cast<UGameWidget>(UIWidget);
		GameWidget->Countdown(CountdownIndex);
		UE_LOG(LogTemp, Warning, TEXT("%s: %d"), *GetName() , CountdownIndex);
	}
	if (CountdownIndex > 4) {
		SetSpeed(700);
	}
}

void ATronPlayerController::MoveLeft(){
	
	if(HasAuthority()){
		if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
			FRotator NewRotation = FRotator(0.0f, -90.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Left;
		}
	}
	else {
		Server_MoveLeft();
	}

}

void ATronPlayerController::MoveUp(){

	if (HasAuthority()) {
		if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
			FRotator NewRotation = FRotator(0.0f, 0.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Up;
		}
	}
	else {
		Server_MoveUp();
	}

}

void ATronPlayerController::MoveDown(){
	if(HasAuthority()){
		if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
			FRotator NewRotation = FRotator(0.0f, 180.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Down;
		}
	}
	else {
		Server_MoveDown();
	}
}

void ATronPlayerController::MoveRight(){
	if(HasAuthority()){
		if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
			FRotator NewRotation = FRotator(0.0f, 90.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Right;
		}
	}
	else {
		Server_MoveRight();
	}
}

void ATronPlayerController::Server_MoveLeft_Implementation()
{
	if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
		FRotator NewRotation = FRotator(0.0f, -90.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Left;
	}
}

void ATronPlayerController::Server_MoveUp_Implementation()
{
	if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
		FRotator NewRotation = FRotator(0.0f, 0.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Up;
	}
}

void ATronPlayerController::Server_MoveDown_Implementation()
{
	if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
		FRotator NewRotation = FRotator(0.0f, 180.0f, 0.0f);
		ControlledPawn->Turn(NewRotation);
		CurrentDirection = EMoveDirection::ED_Down;
	}
}

void ATronPlayerController::Server_MoveRight_Implementation()
{
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
	PossessPawn_Implementation(InPawn);
}

void ATronPlayerController::PossessPawn_Implementation(APawn* InPawn)
{
	ControlledPawn = Cast<APlayerPawn>(InPawn);
	if (!ControlledPawn) UE_LOG(LogTemp, Warning, TEXT("No Pawn"));
	ControlledPawn->speed = 0;
	UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *InPawn->GetName(), *ControlledPawn->GetVelocity().ToString());

	ControlledPawn->OnPossess();
}

void ATronPlayerController::Server_SetSpeed_Implementation(float fspeed){
	ControlledPawn->speed = fspeed;
}

void ATronPlayerController::SetSpeed(float fspeed){
	if (HasAuthority()) {
		ControlledPawn->speed = fspeed;
		UE_LOG(LogTemp, Warning, TEXT("Speed Set"));
		ControlledPawn->OnRep_Speed();
	}
	else {
		Server_SetSpeed(fspeed);
	}
	
}
