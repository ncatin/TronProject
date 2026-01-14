// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include <Components/CapsuleComponent.h>
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"



// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	PMComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	PMComponent->SetUpdatedComponent(RootComponent);

	SpComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SpComponent->bAllowDiscontinuousSpline = true;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CollisionCapsule->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	bDead = false;

	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &APlayerPawn::OnCollision);

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

void APlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerPawn, speed);
}

void APlayerPawn::OnPossess(){
	if(HasAuthority()){
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		EnableInput(PlayerController);
		PMComponent->Velocity = GetActorForwardVector() * speed;
	}
	else {
		Server_OnPossess();
	}
}

void APlayerPawn::Server_OnPossess_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	EnableInput(PlayerController);
	PMComponent->Velocity = GetActorForwardVector() * speed;
}

void APlayerPawn::OnRep_Speed(){
	PMComponent->Velocity = GetActorForwardVector() * speed;
}

void APlayerPawn::GetCurrentPointPosition(){

	SpComponent->SetLocationAtSplinePoint(CurrentSplineIndex, GetActorLocation(), ESplineCoordinateSpace::World);
	LocationEnd = SpComponent->GetLocationAtSplinePoint(CurrentSplineIndex, ESplineCoordinateSpace::World);
	SplineMeshComponents[CurrentSplineMeshIndex]->SetStartAndEnd(LocationStart, FVector::ZeroVector, LocationEnd, FVector::ZeroVector);
}

void APlayerPawn::Turn(FRotator TurnDirection){

	if(HasAuthority()){
		FBox BoundingBox = WallMesh->GetBoundingBox();
		FVector Dimensions = BoundingBox.GetExtent();

		FVector SpawnPoint = LocationEnd;


		AStaticMeshActor* CornerMesh = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnPoint, FRotator::ZeroRotator);
		CornerMesh->SetMobility(EComponentMobility::Movable);
		UStaticMeshComponent* StaticMeshComp = CornerMesh->GetStaticMeshComponent();
		StaticMeshComp->SetStaticMesh(WallMesh);
		StaticMeshComp->SetCastShadow(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		FSplinePoint NewPoint;

		NewPoint.Position = GetActorLocation();
		NewPoint.Rotation = TurnDirection;
		NewPoint.InputKey = ++CurrentSplineIndex;
		NewPoint.Type = ESplinePointType::Linear;

		SpComponent->AddPoint(NewPoint, ESplineCoordinateSpace::World);
		SpComponent->GetLocationAndTangentAtSplinePoint(CurrentSplineIndex - 1, LocationStart, TangentStart, ESplineCoordinateSpace::World);

		CreateSplineMesh();
		CurrentSplineMeshIndex++;

		SetActorRotation(TurnDirection);
		PMComponent->Velocity = GetActorForwardVector() * speed;
		UE_LOG(LogTemp, Warning, TEXT("Rotated"));
	}
	else {
		Server_Turn(TurnDirection);
	}
}

void APlayerPawn::Server_Turn_Implementation(FRotator TurnDirection)
{
	FBox BoundingBox = WallMesh->GetBoundingBox();
	FVector Dimensions = BoundingBox.GetExtent();

	FVector SpawnPoint = LocationEnd;


	AStaticMeshActor* CornerMesh = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnPoint, FRotator::ZeroRotator);
	CornerMesh->SetMobility(EComponentMobility::Movable);
	UStaticMeshComponent* StaticMeshComp = CornerMesh->GetStaticMeshComponent();
	StaticMeshComp->SetStaticMesh(WallMesh);
	StaticMeshComp->SetCastShadow(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FSplinePoint NewPoint;

	NewPoint.Position = GetActorLocation();
	NewPoint.Rotation = TurnDirection;
	NewPoint.InputKey = ++CurrentSplineIndex;
	NewPoint.Type = ESplinePointType::Linear;

	SpComponent->AddPoint(NewPoint, ESplineCoordinateSpace::World);
	SpComponent->GetLocationAndTangentAtSplinePoint(CurrentSplineIndex - 1, LocationStart, TangentStart, ESplineCoordinateSpace::World);

	CreateSplineMesh();
	CurrentSplineMeshIndex++;

	SetActorRotation(TurnDirection);
	PMComponent->Velocity = GetActorForwardVector() * speed;
	UE_LOG(LogTemp, Warning, TEXT("Rotated"));
}

void APlayerPawn::CreateSplineMesh(){

	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SplineMesh->SetupAttachment(SpComponent);
	SplineMesh->bCastDynamicShadow = false;
	if (WallMesh) SplineMesh->SetStaticMesh(WallMesh);

	SplineMesh->SetStartScale(FVector2D(1,1));
	SplineMesh->SetEndScale(FVector2D(1, 1));

	SplineMesh->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd);
	SplineMesh->RegisterComponent();

	SplineMeshComponents.Add(SplineMesh);

}

void APlayerPawn::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	if (OtherActor == this) {
		return;
	}
	bDead = true;

	PMComponent->Velocity = GetActorForwardVector() * 0;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	DisableInput(PlayerController);

	if (ExplosionSystem) {
		FVector SpawnLocation = this->GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionSystem, SpawnLocation, FRotator::ZeroRotator);
	}
	MeshComponent->SetVisibility(false);
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


