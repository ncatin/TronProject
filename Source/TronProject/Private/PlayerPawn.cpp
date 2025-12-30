// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"



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
	SpComponent->bAllowDiscontinuousSpline = true;
	
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

	SpComponent->SetSplinePointType(0, ESplinePointType::Linear);
	SpComponent->SetSplinePointType(1, ESplinePointType::Linear);

	LocationStart = SpComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	LocationEnd = SpComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);

	CreateSplineMesh();

	RegisterAllComponents();
	

}

void APlayerPawn::GetCurrentPointPosition(){

	SpComponent->SetLocationAtSplinePoint(CurrentSplineIndex, GetActorLocation(), ESplineCoordinateSpace::World);
	LocationEnd = SpComponent->GetLocationAtSplinePoint(CurrentSplineIndex, ESplineCoordinateSpace::World);
	SplineMeshComponents[CurrentSplineMeshIndex]->SetStartAndEnd(LocationStart, FVector::ZeroVector, LocationEnd, FVector::ZeroVector);
}

void APlayerPawn::Turn(FRotator TurnDirection){

	FBox BoundingBox = WallMesh->GetBoundingBox();
	FVector Dimensions = BoundingBox.GetExtent();

	FVector SpawnPoint = LocationEnd;


	AStaticMeshActor* CornerMesh = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnPoint, FRotator::ZeroRotator);
	CornerMesh->SetMobility(EComponentMobility::Movable);
	UStaticMeshComponent* StaticMeshComp = CornerMesh->GetStaticMeshComponent();
	StaticMeshComp->SetStaticMesh(WallMesh);
	

	FSplinePoint NewPoint;

	NewPoint.Position = GetActorLocation();
	NewPoint.Rotation = TurnDirection;
	NewPoint.InputKey = ++CurrentSplineIndex;
	NewPoint.Type = ESplinePointType::Linear;
	
	SpComponent->AddPoint(NewPoint, ESplineCoordinateSpace::World);
	SpComponent->GetLocationAndTangentAtSplinePoint(CurrentSplineIndex-1, LocationStart, TangentStart, ESplineCoordinateSpace::World);
	
	CreateSplineMesh();
	CurrentSplineMeshIndex++;

	SetActorRotation(TurnDirection);
	PMComponent->Velocity = GetActorForwardVector() * 500;
	UE_LOG(LogTemp, Warning, TEXT("Rotated"));
}

void APlayerPawn::CreateSplineMesh(){

	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetupAttachment(SpComponent);
	SplineMesh->bCastDynamicShadow = false;
	if (WallMesh) SplineMesh->SetStaticMesh(WallMesh);

	SplineMesh->SetStartScale(FVector2D(1,1));
	SplineMesh->SetEndScale(FVector2D(1, 1));

	SplineMesh->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd);
	SplineMesh->RegisterComponent();

	SplineMeshComponents.Add(SplineMesh);

}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


