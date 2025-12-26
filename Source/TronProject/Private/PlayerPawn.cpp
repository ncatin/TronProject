// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"



// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	PMComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	PMComponent->SetUpdatedComponent(RootComponent);

	SpComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		EnableInput(PlayerController);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Error"));
	} 

	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(RepeatingHandle, this, &APlayerPawn::GetCurrentPointPosition, PointCaptureRate, true);

}

void APlayerPawn::OnConstruction(const FTransform& Transform){
	Super::OnConstruction(Transform);

	SpComponent->SetLocationAtSplinePoint(0, GetActorLocation(), ESplineCoordinateSpace::World);
	SpComponent->SetRotationAtSplinePoint(0, GetActorRotation(), ESplineCoordinateSpace::World);

	SpComponent->GetLocationAndTangentAtSplinePoint(0, LocationStart, TangentStart, ESplineCoordinateSpace::World);
	SpComponent->GetLocationAndTangentAtSplinePoint(1, LocationEnd, TangentEnd, ESplineCoordinateSpace::World);

	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetupAttachment(SpComponent);
	SplineMesh->bCastDynamicShadow = false;
	if (WallMesh) SplineMesh->SetStaticMesh(WallMesh);

	SplineMesh->SetStartScale(FVector2D(1, 5));
	SplineMesh->SetEndScale(FVector2D(1, 5));

	SplineMesh->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd);

	SplineMeshComponents.Add(SplineMesh);

	RegisterAllComponents();
	

}

void APlayerPawn::GetCurrentPointPosition(){

	SpComponent->SetLocationAtSplinePoint(CurrentSplineIndex, GetActorLocation(), ESplineCoordinateSpace::World);
	SpComponent->GetLocationAndTangentAtSplinePoint(CurrentSplineIndex, LocationEnd, TangentEnd, ESplineCoordinateSpace::World);
	SplineMeshComponents[CurrentSplineMeshIndex]->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd);
	SplineMeshComponents[CurrentSplineMeshIndex]->RegisterComponent();
	UE_LOG(LogTemp, Warning, TEXT("Getting Spline Point"));
}

void APlayerPawn::Turn(FRotator TurnDirection){
	SetActorRotation(TurnDirection);
	PMComponent->Velocity = GetActorForwardVector() * 500;
	UE_LOG(LogTemp, Warning, TEXT("Rotated"));
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


