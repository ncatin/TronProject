// Fill out your copyright notice in the Description page of Project Settings.


#include "TronGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "TronPlayerController.h"
#include "StaticCameraActor.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"

void ATronGameMode::BeginPlay(){

}

void ATronGameMode::Ready(){
	ReadyPlayers++;
	if (ReadyPlayers == 2) {
		//Start UI countdown
	}
}

void ATronGameMode::SetBegin(bool flag) { StartFlag = flag; }

bool ATronGameMode::GetBegin() { return StartFlag; }

void ATronGameMode::StartMatch(){
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATronPlayerController::StaticClass(), Players);
	for (AActor* Player : Players) {
		ATronPlayerController* PlayerController = Cast<ATronPlayerController>(Player);
		PlayerController->SetSpeed(700);
	}
}

void ATronGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer){
	
	if (NewPlayer) {
		
		AActor* StartPoint = FindPlayerStart(NewPlayer);
		if (StartPoint) {
			FVector SpawnLocation = StartPoint->GetActorLocation();
			FRotator SpawnRotation;
			if (SpawnLocation.Y > 0) {
				SpawnRotation = FRotator(0.0f, -90.0f, 0.0f);
			}
			else {
				SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
			}
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NewPlayer;

			APawn* NewPawn;
			UClass* PlayerClass;
			if (JoinedPlayers == 0) {
				PlayerClass = PlayerOne;
			}else{
				PlayerClass = PlayerTwo;
			}
			NewPawn = GetWorld()->SpawnActor<APawn>(PlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
			JoinedPlayers++;

			if (NewPawn) {
				NewPlayer->Possess(NewPawn);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Error: Posession failed"));
			}
			StartPoint->Tags.Remove(FName("Available"));

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
