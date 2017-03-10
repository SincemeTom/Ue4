// Fill out your copyright notice in the Description page of Project Settings.

#include "TestThirdProject.h"
#include "MyProjectile.h"


AMyProjectile::AMyProjectile()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AMyProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	InitialLifeSpan = 3.0f;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = true;
	
}
void AMyProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
void AMyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if (OtherActor != Instigator)
	{
		Destroy();
	}
	
}



// Called when the game starts or when spawned
void AMyProjectile::BeginPlay()
{
	Super::BeginPlay();	
}
void AMyProjectile::OnRep_Instigator()
{
	
	ACharacter* pOwnerCharacter = Cast<ACharacter>(Instigator);
	if (pOwnerCharacter)
	{
		GetProjectileMesh()->IgnoreActorWhenMoving(pOwnerCharacter, true);
		pOwnerCharacter->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);
	}
}

void AMyProjectile::OnRep_Owner()
{

}
// Called every frame
void AMyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


