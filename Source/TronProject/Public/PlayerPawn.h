// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "PlayerPawn.generated.h"

class UFloatingPawnMovement;
class UProjectileMovementComponent;
class ATronPlayerController;
class USplineComponent;
class USplineMeshComponent;

UCLASS()
class TRONPROJECT_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 CurrentSplineIndex = 1;
	int32 CurrentSplineMeshIndex = 0;

	FTimerHandle RepeatingHandle;

	float PointCaptureRate = 0.01f;

	TArray<USplineMeshComponent*> SplineMeshComponents;

	FVector LocationStart, TangentStart, LocationEnd, TangentEnd;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* PMComponent;

	UPROPERTY(EditAnywhere)
	USplineComponent* SpComponent;

	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh;

	UFUNCTION()
	void GetCurrentPointPosition();

	UFUNCTION()
	void Turn(FRotator TurnDirection);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
