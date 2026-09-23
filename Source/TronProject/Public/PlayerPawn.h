// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Components/SplineComponent.h"
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


USTRUCT()
struct FTronSplinePoint {
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector StartTangent;

	UPROPERTY()
	FVector EndTangent;

	UPROPERTY()
	TEnumAsByte<ESplinePointType::Type> PointType;

	FTronSplinePoint()
		: Location(FVector::ZeroVector),
		StartTangent(FVector::ZeroVector),
		EndTangent(FVector::ZeroVector),
		PointType(ESplinePointType::Linear)
	{}
};


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

	APlayerController* PlayerController;

	UPROPERTY(Replicated)
	int32 CurrentSplineIndex = 1;

	UPROPERTY(Replicated)
	int32 CurrentSplineMeshIndex = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	bool bDead = false;

	FTimerHandle RepeatingHandle;

	UPROPERTY(Replicated)
	float PointCaptureRate = 0.01f;

	UPROPERTY(Replicated)
	TArray<USplineMeshComponent*> SplineMeshComponents;

	UPROPERTY(Replicated)
	USplineMeshComponent* CurrentSplineMesh;

	UPROPERTY(Replicated)
	FVector LocationStart;

	UPROPERTY(Replicated)
	FVector TangentStart;

	UPROPERTY(ReplicatedUsing = OnRep_LocationEnd)
	FVector LocationEnd;

	UPROPERTY(Replicated)
	FVector TangentEnd;

	/*UPROPERTY(ReplicatedUsing = OnRep_SplinePoints)
	TArray<FTronSplinePoint> TronSplinePoints;

	UFUNCTION()
	void OnRep_SplinePoints();*/

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	UPROPERTY(ReplicatedUsing = OnRep_Speed, BlueprintReadOnly, EditAnywhere)
	int32 speed = 1500;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector velocity;

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
	void OnRep_Dead();

	UFUNCTION()
	void OnRep_Speed();

	UFUNCTION()
	void GetCurrentPointPosition();

	UFUNCTION()
	void Rep_GetCurrentPointPosition();

	UFUNCTION(Server, Reliable)
	void Server_GetCurrentPointPosition();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GetCurrentPointPosition();

	UFUNCTION()
	void Turn(FRotator TurnDirection);

	UFUNCTION(Server, Reliable)
	void Server_Turn(FRotator TurnDirection);

	UFUNCTION()
	void CreateSplineMesh();

	UFUNCTION()
	void Rep_CreateSplineMesh();

	UFUNCTION(Server, Reliable)
	void Server_CreateSplineMesh();

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPossess();

	UFUNCTION(Server, Reliable)
	void Server_OnPossess();

	UFUNCTION()
	void OnRep_LocationEnd();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
