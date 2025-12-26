// Fill out your copyright notice in the Description page of Project Settings.


#include "TronGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "TronPlayerController.h"
#include "StaticCameraActor.h"

void ATronGameMode::BeginPlay(){
	AStaticCameraActor* StaticCamera = GetWorld()->SpawnActor<AStaticCameraActor>(FVector(0.0f, 0.0f, 2000.0f), FRotator(-90.0f, 0.0f, 0.0f));

	ATronPlayerController* PlayerController = Cast<ATronPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PlayerController) {
		PlayerController->SetViewTarget(StaticCamera);
	}
}

void ATronGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer){
	
	if (NewPlayer) {
		AActor* StartPoint = FindPlayerStart(NewPlayer);
		if (StartPoint) {
			FVector SpawnLocation = StartPoint->GetActorLocation();
			FRotator SpawnRotation = FRotator(0.0f, -90.0f, 0.0f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NewPlayer;

			APawn* NewPawn = GetWorld()->SpawnActor<APawn>(this->DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);

			if (NewPawn) {
				NewPlayer->Possess(NewPawn);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Error"));
			}

		}
	}
}
