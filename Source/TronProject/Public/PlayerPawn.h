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
class UCapsuleComponent;
class UNiagaraSystem;
class UAudioComponent;
class USpringArmComponent;
class UCameraComponent;


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
	
	bool bDead = false;

	FTimerHandle RepeatingHandle;

	float PointCaptureRate = 0.01f;

	TArray<USplineMeshComponent*> SplineMeshComponents;

	FVector LocationStart, TangentStart, LocationEnd, TangentEnd;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	int32 speed = 700;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* PMComponent;

	UPROPERTY(EditAnywhere)
	USplineComponent* SpComponent;

	UPROPERTY(EditAnywhere)
	UStaticMesh* WallMesh;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CollisionCapsule;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ExplosionSystem;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* DeathSoundEffect;

	UFUNCTION()
	void GetCurrentPointPosition();

	UFUNCTION()
	void Turn(FRotator TurnDirection);

	UFUNCTION()
	void CreateSplineMesh();

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnPossess();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
