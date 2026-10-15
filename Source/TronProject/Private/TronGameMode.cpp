// Fill out your copyright notice in the Description page of Project Settings.


#include "TronGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "TronPlayerController.h"
#include "StaticCameraActor.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include <Net/UnrealNetwork.h>
#include "TronGameState.h"
#include "PlayerPawn.h"

void ATronGameMode::BeginPlay(){

}

void ATronGameMode::SpawnPawns(ATronPlayerController* P){
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = P;

	APlayerPawn* NewPawn = Cast<APlayerPawn>(GetWorld()->SpawnActor<APawn>(P->PlayerPawnClass, P->SpawnPoint, P->SpawnRotation, SpawnParams));
	if (NewPawn) {
		P->ServerSide_PossesPawn(NewPawn);
	}
}

void ATronGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer){
	
	ATronPlayerController* NewTronPlayer = Cast<ATronPlayerController>(NewPlayer);
	if (NewTronPlayer) {
		NewTronPlayer->GameMode = this;
		NewTronPlayer->lives = 3;
		PlayerControllers.Add(NewTronPlayer);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *NewTronPlayer->GetName());
		AActor* StartPoint = FindPlayerStart(NewTronPlayer);
		if (StartPoint) {
			NewTronPlayer->SpawnPoint = StartPoint->GetActorLocation();
			
			if (StartPoint->ActorHasTag(FName("Right"))) {
				NewTronPlayer->SpawnRotation = FRotator(0.0f, -90.0f, 0.0f);
				NewTronPlayer->CurrentDirection = EMoveDirection::ED_Left;
			}
			else {
				NewTronPlayer->SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
				NewTronPlayer->CurrentDirection = EMoveDirection::ED_Right;
			}
			
			if (JoinedPlayers == 0) {
				NewTronPlayer->PlayerPawnClass = PlayerOne;
				NewTronPlayer->PlayerID = FName("P1");
			}else{
				NewTronPlayer->PlayerPawnClass = PlayerTwo;
				NewTronPlayer->PlayerID = FName("P2");
			}

			SpawnPawns(NewTronPlayer);

			JoinedPlayers++;
			
			if (JoinedPlayers == 2) {
				StartTimer();
			}
			
			else {
				UE_LOG(LogTemp, Warning, TEXT("Error: Posession failed"));
			}
			StartPoint->Tags.Remove(FName("Available"));

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Error: Can't find start point"));
		}
	}
}

AActor* ATronGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), "Available", PlayerStarts);
	AActor* ChoosenPlayerStart = PlayerStarts[0];
	if (ChoosenPlayerStart) {
		UE_LOG(LogTemp, Warning, TEXT("Player Start Found"));
		return ChoosenPlayerStart;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Error: No Player Start"));
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATronGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATronGameMode, Countdown);
}

void ATronGameMode::StartTimer() {
	UE_LOG(LogTemp, Warning, TEXT("Starting Timer"));
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(RepeatingHandle, this, &ATronGameMode::UpdateCountdown, 1, true);
}

void ATronGameMode::UpdateCountdown() {
	UE_LOG(LogTemp, Warning, TEXT("counting down"));
	if (Countdown <= 5) {
		Countdown++;
		for (ATronPlayerController* PlayerController : PlayerControllers) {
			PlayerController->GameStateCountdown(Countdown);
		}
	}
	else {
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(RepeatingHandle);
	}


}

void ATronGameMode::EndMatch(ATronPlayerController* DeadPlayer){
	if (!bEndingMatch) {
		bEndingMatch = true;

		if (DeadPlayer->lives == 0) {
			//end game
		}
		for (ATronPlayerController* P : PlayerControllers) {
			P->UnPossess();
		}

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		FTimerDelegate Delegate;

		Delegate.BindUObject(this, &ATronGameMode::ResetGame, DeadPlayer->PlayerID);

		TimerManager.SetTimer(RepeatingHandle, Delegate, 3, false);

	}
	
	
}

void ATronGameMode::ResetGame(FName PlayerID){
	for (ATronPlayerController* P : PlayerControllers) {
		P->DeductLife(PlayerID);
		P->ControlledPawn->Destroy();
		SpawnPawns(P);
	}
	bEndingMatch = false;
}

void ATronGameMode::Tick(float DeltaTime){

}
