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

void ATronGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer){
	
	ATronPlayerController* NewTronPlayer = Cast<ATronPlayerController>(NewPlayer);
	if (NewTronPlayer) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *NewTronPlayer->GetName());
		AActor* StartPoint = FindPlayerStart(NewTronPlayer);
		if (StartPoint) {
			FVector SpawnLocation = StartPoint->GetActorLocation();
			FRotator SpawnRotation;
			if (StartPoint->ActorHasTag(FName("Right"))) {
				SpawnRotation = FRotator(0.0f, -90.0f, 0.0f);
				NewTronPlayer->CurrentDirection = EMoveDirection::ED_Left;
			}
			else {
				SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
				NewTronPlayer->CurrentDirection = EMoveDirection::ED_Right;
			}
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NewTronPlayer;

			APawn* NewPawn;
			UClass* PlayerClass;
			if (JoinedPlayers == 0) {
				PlayerClass = PlayerOne;
			}else{
				PlayerClass = PlayerTwo;
			}
			NewPawn = GetWorld()->SpawnActor<APawn>(PlayerClass, SpawnLocation, SpawnRotation, SpawnParams);
			UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *NewPawn->GetName(), *NewTronPlayer->GetName());
			JoinedPlayers++;

			if (NewPawn) {
				NewTronPlayer->ServerSide_PossesPawn(NewPawn);
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
