// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
#include "Kismet/GameplayStatics.h"



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

	PMComponent->bInterpMovement = true;
	PMComponent->bInterpRotation = true;
	

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
	TimerManager.SetTimer(RepeatingHandle, this, &APlayerPawn::Rep_GetCurrentPointPosition, PointCaptureRate, true);

}

void APlayerPawn::OnConstruction(const FTransform& Transform){
	Super::OnConstruction(Transform);
	
	SpComponent->SetLocationAtSplinePoint(0, GetActorLocation(), ESplineCoordinateSpace::Local);
	SpComponent->SetLocationAtSplinePoint(1, GetActorLocation(), ESplineCoordinateSpace::Local);

	SpComponent->SetSplinePointType(0, ESplinePointType::Linear);
	SpComponent->SetSplinePointType(1, ESplinePointType::Linear);

	SpComponent->UpdateSpline();

	LocationStart = SpComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	LocationEnd = SpComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);

	Rep_CreateSplineMesh();

	RegisterAllComponents();
	

}

void APlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerPawn, speed);
	DOREPLIFETIME(APlayerPawn, CurrentSplineIndex);
	DOREPLIFETIME(APlayerPawn, CurrentSplineMeshIndex);
	DOREPLIFETIME(APlayerPawn, bDead);
	DOREPLIFETIME(APlayerPawn, PointCaptureRate);
	DOREPLIFETIME(APlayerPawn, SplineMeshComponents);
	DOREPLIFETIME(APlayerPawn, LocationStart);
	DOREPLIFETIME(APlayerPawn, TangentStart);
	DOREPLIFETIME(APlayerPawn, LocationEnd);
	DOREPLIFETIME(APlayerPawn, TangentEnd);
	DOREPLIFETIME(APlayerPawn, CurrentSplineMesh);
}

void APlayerPawn::OnPossess(){
	if(HasAuthority()){
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0);
		EnableInput(PlayerController);
		velocity = GetActorForwardVector() * speed;
	}
	else {
		Server_OnPossess();
	}
}

void APlayerPawn::Server_OnPossess_Implementation()
{
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	EnableInput(PlayerController);
	velocity = GetActorForwardVector() * speed;
}

void APlayerPawn::OnRep_Speed(){
	velocity = GetActorForwardVector() * speed;
	PMComponent->UpdateComponentVelocity();
	UE_LOG(LogTemp, Warning, TEXT("Velocity Changed"));
	
}

void APlayerPawn::GetCurrentPointPosition(){

	LocationEnd = GetActorLocation();
	OnRep_LocationEnd();
}

void APlayerPawn::OnRep_LocationEnd() {
	
	SpComponent->RemoveSplinePoint(CurrentSplineIndex, true);

	FSplinePoint NewPoint;

	NewPoint.Position = LocationEnd;
	NewPoint.InputKey = CurrentSplineIndex;
	NewPoint.Type = ESplinePointType::Linear;

	SpComponent->AddPoint(NewPoint, ESplineCoordinateSpace::Local);

	if(SplineMeshComponents[CurrentSplineMeshIndex]){ //create splinemesh vairable equal to current spline, update end point, and replace current spline mesh with it
		
		CurrentSplineMesh = SplineMeshComponents[CurrentSplineMeshIndex];
		//if (!CurrentSplineMesh->IsVisible()) CurrentSplineMesh->SetVisibility(true);
		CurrentSplineMesh->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd);
		SplineMeshComponents[CurrentSplineMeshIndex] = CurrentSplineMesh;

	}
	SpComponent->UpdateSpline();
	
}

void APlayerPawn::Rep_GetCurrentPointPosition(){
	if (HasAuthority()) {
		FVector NextPosition = GetActorLocation() + velocity * PointCaptureRate;
		SetActorLocation(NextPosition);
		GetCurrentPointPosition();
		OnRep_LocationEnd(); 
	}
	else {
		Server_GetCurrentPointPosition();
	}
}

void APlayerPawn::Server_GetCurrentPointPosition_Implementation(){
	GetCurrentPointPosition();
	OnRep_LocationEnd();
}

void APlayerPawn::Multicast_GetCurrentPointPosition_Implementation(){
	GetCurrentPointPosition();
}

void APlayerPawn::Turn(FRotator TurnDirection){

	if(HasAuthority()){
		FBox BoundingBox = WallMesh->GetBoundingBox();
		FVector Dimensions = BoundingBox.GetExtent();

		FVector SpawnPoint = LocationEnd;


		AStaticMeshActor* CornerMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform(FRotator::ZeroRotator, SpawnPoint));
		CornerMesh->SetRootComponent(CornerMesh->GetStaticMeshComponent());
		CornerMesh->SetActorHiddenInGame(true);
		CornerMesh->SetMobility(EComponentMobility::Movable);
		CornerMesh->SetReplicates(true);
		CornerMesh->SetReplicateMovement(true);
		UStaticMeshComponent* StaticMeshComp = CornerMesh->GetStaticMeshComponent();
		StaticMeshComp->SetStaticMesh(WallMesh);
		StaticMeshComp->SetCastShadow(false);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		StaticMeshComp->SetCollisionProfileName(TEXT("OverlapAll"));
		StaticMeshComp->SetGenerateOverlapEvents(true);
		StaticMeshComp->SetIsReplicated(true);
		CornerMesh->SetActorHiddenInGame(false);

		FSplinePoint NewPoint;

		NewPoint.Position = GetActorLocation();
		NewPoint.Rotation = TurnDirection;
		NewPoint.InputKey = ++CurrentSplineIndex;
		NewPoint.Type = ESplinePointType::Linear;

		SpComponent->AddPoint(NewPoint, ESplineCoordinateSpace::Local);
		SpComponent->GetLocationAndTangentAtSplinePoint(CurrentSplineIndex - 1, LocationStart, TangentStart, ESplineCoordinateSpace::Local);

		Rep_CreateSplineMesh();
		CurrentSplineMeshIndex++;

		SetActorRotation(TurnDirection);
		OnRep_Speed();
		
		PMComponent->SetUpdatedComponent(GetRootComponent());
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
	StaticMeshComp->SetCollisionProfileName(TEXT("OverlapAll"));
	StaticMeshComp->SetGenerateOverlapEvents(true);
	FSplinePoint NewPoint;

	NewPoint.Position = GetActorLocation();
	NewPoint.Rotation = TurnDirection;
	NewPoint.InputKey = ++CurrentSplineIndex;
	NewPoint.Type = ESplinePointType::Linear;

	SpComponent->AddPoint(NewPoint, ESplineCoordinateSpace::Local);
	SpComponent->GetLocationAndTangentAtSplinePoint(CurrentSplineIndex - 1, LocationStart, TangentStart, ESplineCoordinateSpace::Local);

	Rep_CreateSplineMesh();
	CurrentSplineMeshIndex++;

	SetActorRotation(TurnDirection);
	OnRep_Speed();
}

void APlayerPawn::CreateSplineMesh(){

	UE_LOG(LogTemp, Warning, TEXT("Creating Spline Mesh"));

	USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
	UE_LOG(LogTemp, Warning, TEXT("MeshLocation: %f %f %f"), SplineMesh->GetRelativeLocation().X, SplineMesh->GetRelativeLocation().Y, SplineMesh->GetRelativeLocation().Z);
	SplineMesh->RegisterComponent();
	SplineMesh->SetVisibility(false);
	SplineMesh->SetMobility(EComponentMobility::Movable);
	SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SplineMesh->SetCollisionProfileName(TEXT("OverlapAll"));
	SplineMesh->SetGenerateOverlapEvents(true);
	if (WallMesh) SplineMesh->SetStaticMesh(WallMesh);
	SplineMesh->SetupAttachment(SpComponent);
	SplineMesh->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd, true);
	UE_LOG(LogTemp, Warning, TEXT("LocationStart: %f, %f, %f LocationEnd: %f, %f, %f"), LocationStart.X, LocationStart.Y, LocationStart.Z, LocationEnd.X, LocationEnd.Y, LocationEnd.Z);
	
	
	SplineMesh->bCastDynamicShadow = false;
	SplineMesh->SetIsReplicated(true);
	

	SplineMesh->SetStartScale(FVector2D(1,1));
	SplineMesh->SetEndScale(FVector2D(1, 1));
	

	SplineMeshComponents.Add(SplineMesh);
	SplineMesh->SetVisibility(true);
}

void APlayerPawn::Rep_CreateSplineMesh(){

	if (HasAuthority()) {
		CreateSplineMesh();
	}
	else {
		Server_CreateSplineMesh();
	}
}

void APlayerPawn::Server_CreateSplineMesh_Implementation(){
	CreateSplineMesh();
}

void APlayerPawn::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){

	if (OtherActor != this) {
		speed = 0;
		OnRep_Speed();

		UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor: %s"), *OtherComp->GetName());
		bDead = true;
		

		if (ExplosionSystem) {
			FVector SpawnLocation = this->GetActorLocation();
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionSystem, SpawnLocation, FRotator::ZeroRotator);
		}

		MeshComponent->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("Player %s dead"), *this->GetName());
	}
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*UE_LOG(LogTemp, Warning, TEXT("Client Velocity: %f %f %f "), PMComponent->Velocity.X, PMComponent->Velocity.Y, PMComponent->Velocity.Z);
	UE_LOG(LogTemp, Warning, TEXT("Client Speed: %d "), speed);*/
}


