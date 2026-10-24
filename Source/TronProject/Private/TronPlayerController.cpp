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


	if (IsLocalPlayerController()) {
		UIWidget = CreateWidget<UUserWidget>(this, UIClass);
		UIWidget->AddToViewport();
	}
	
}

void ATronPlayerController::Tick(float DeltaTime){

}

void ATronPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Pawn called"));
}

void ATronPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATronPlayerController, lives);
}

void ATronPlayerController::OnUnPossess(){
	Super::OnUnPossess();
	UE_LOG(LogTemp, Warning, TEXT("Overriden unpossess called"));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->RemoveMappingContext(DefaultMappingContext);
	}
	
}

void ATronPlayerController::Client_DeductLife_Implementation(FName PID){
	bool bSuccess;
	UE_LOG(LogTemp, Warning, TEXT("Deduct Life running for client"));
	if (UIWidget) {
		UGameWidget* GameWidget = Cast<UGameWidget>(UIWidget);
		if (PID == "P1") {
			
			int32 index = GameWidget->P1HorizontalBox->GetChildrenCount() - 1;

			UE_LOG(LogTemp, Warning, TEXT("P1 Box Index: %d"), index);
			bSuccess = GameWidget->P1HorizontalBox->RemoveChildAt(index);
		}
		else if (PID == "P2") {
			int32 index = GameWidget->P2HorizontalBox->GetChildrenCount() - 1;

			UE_LOG(LogTemp, Warning, TEXT("P1 Box Index: %d"), index);
			bSuccess = GameWidget->P2HorizontalBox->RemoveChildAt(index);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No PlayerID"));
			bSuccess = false;
		}
		UE_LOG(LogTemp, Warning, TEXT("Deduct Life For Client: %s"), bSuccess ? TEXT("Success") : TEXT("Failure"));
	}
}

void ATronPlayerController::Server_DeductLife_Implementation(FName PID){
	bool bSuccess;
	UE_LOG(LogTemp, Warning, TEXT("Deduct Life running"));
	if (UIWidget) {
		UGameWidget* GameWidget = Cast<UGameWidget>(UIWidget);
		if (PID == "P1") {
			/*int32 index = GameWidget->P1HorizontalBox->GetChildrenCount() - 1;
			bSuccess = GameWidget->P1HorizontalBox->RemoveChildAt(index);*/
		}
		else if (PID == "P2") {
			/*int32 index = GameWidget->P2HorizontalBox->GetChildrenCount() - 1;
			bSuccess = GameWidget->P2HorizontalBox->RemoveChildAt(index);*/
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No PlayerID"));
		}
		UE_LOG(LogTemp, Warning, TEXT("Deduct Life: %s"), bSuccess ? TEXT("Success") : TEXT("Failure"));
	}
}

void ATronPlayerController::Server_OnDeath_Implementation(){
	UE_LOG(LogTemp, Warning, TEXT("Calling OnDeath"));
	lives--;
	bDead = true;
	GameMode->EndMatch(this);
}


void ATronPlayerController::ServerSide_PossesPawn_Implementation(APawn* InPawn){
	if (InPawn && HasAuthority()) {
		Possess(InPawn);
	}
}


void ATronPlayerController::GameStateCountdown_Implementation(int CountdownIndex){
	
	if (UIWidget) {
		UGameWidget* GameWidget = Cast<UGameWidget>(UIWidget);
		GameWidget->Countdown(CountdownIndex);
	}
	if (CountdownIndex > 4) {
		SetSpeed(700);
		bMatchOver = false;
	}
}

void ATronPlayerController::DeductLife(FName PID){
	if (GetLocalRole() < ROLE_Authority) {
		Server_DeductLife(PID);
	}
	else {
		Client_DeductLife_Implementation(PID);
	}
}

void ATronPlayerController::Server_EndMatch_Implementation(){
	SetSpeed(0);
	bMatchOver = true;

}

void ATronPlayerController::MoveLeft(){
	if (!bMatchOver) {
		if (HasAuthority()) {
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
	
}

void ATronPlayerController::MoveUp(){

	if (!bMatchOver) {
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

}

void ATronPlayerController::MoveDown(){
	if (!bMatchOver) {
		if (HasAuthority()) {
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
	
}

void ATronPlayerController::MoveRight(){
	if (!bMatchOver) {
		if (HasAuthority()) {
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
	
}

void ATronPlayerController::Server_MoveLeft_Implementation()
{
	if (!bMatchOver) {
		if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
			FRotator NewRotation = FRotator(0.0f, -90.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Left;
		}
	}
	
}

void ATronPlayerController::Server_MoveUp_Implementation()
{
	if (!bMatchOver) {
		if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
			FRotator NewRotation = FRotator(0.0f, 0.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Up;
		}
	}
	
}

void ATronPlayerController::Server_MoveDown_Implementation()
{
	if (!bMatchOver) {
		if (CurrentDirection == EMoveDirection::ED_Left || CurrentDirection == EMoveDirection::ED_Right) {
			FRotator NewRotation = FRotator(0.0f, 180.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Down;
		}
	}
	
}

void ATronPlayerController::Server_MoveRight_Implementation()
{
	if (!bMatchOver) {
		if (CurrentDirection == EMoveDirection::ED_Up || CurrentDirection == EMoveDirection::ED_Down) {
			FRotator NewRotation = FRotator(0.0f, 90.0f, 0.0f);
			ControlledPawn->Turn(NewRotation);
			CurrentDirection = EMoveDirection::ED_Right;
		}
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

	ControlledPawn->OnPossess();
}

void ATronPlayerController::Server_SetSpeed_Implementation(float fspeed){
	ControlledPawn->speed = fspeed;
	
}

void ATronPlayerController::SetSpeed(float fspeed){
	if (HasAuthority()) {
		ControlledPawn->speed = fspeed;
		ControlledPawn->OnRep_Speed();
	}else{
		Server_SetSpeed(fspeed);
	}
	
}

void ATronPlayerController::OnDeath(){
	
	if (GetLocalRole() < ROLE_Authority) {
		Server_OnDeath();
	}
	else {
		Server_OnDeath_Implementation();
	}
}

void ATronPlayerController::EndMatch(){
	if (GetLocalRole() < ROLE_Authority) {
		Server_EndMatch();
	}
	else {
		Server_EndMatch_Implementation();
	}
}
