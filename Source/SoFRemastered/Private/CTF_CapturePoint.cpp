// Fill out your copyright notice in the Description page of Project Settings.


#include "CTF_CapturePoint.h"
#include "BaseFlag.h"
#include "MasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "SPlayerState.h"

// Sets default values
ACTF_CapturePoint::ACTF_CapturePoint()
{
 

	PrimaryActorTick.bCanEverTick = false;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);//this is a collision radius for the flag to be picked up
	RootComponent = SphereComp;//that becomes the root component

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));//designs are attached to it 
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->SetRelativeRotation(FRotator(90, 0.f, 0.f));//sets the design in the proper spot of the sphere
	DecalComp->DecalSize = FVector(64, 75, 75);
	SetReplicates(true);

}

// Called when the game starts or when spawned
void ACTF_CapturePoint::BeginPlay()
{
	Super::BeginPlay();
	
	//SetReplicates(true);
	Respawn();

}

void ACTF_CapturePoint::GiveFlag(AMasterCharacter* Target)
{
	if (FlagInstance)
	{
		FlagInstance->AttachFlagToPlayer(Target);
	}
}

// Called every frame
void ACTF_CapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACTF_CapturePoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	AMasterCharacter* OverlapChar = Cast<AMasterCharacter>(OtherActor);
	if (OverlapChar)
	{
		if (OverlapChar->TeamNum != TeamNum)
		{
			GiveFlag(OverlapChar);
		}
	}
}

void ACTF_CapturePoint::Respawn()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (FlagClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PowerupClass is nullptr!"), *GetName());
			return;
		}
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FlagInstance = GetWorld()->SpawnActor<ABaseFlag>(FlagClass, GetTransform(), Params);
		FlagInstance->TeamNum = TeamNum;
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::FromInt(TeamNum));
	}
	else
	{
		ServerRespawn();
	}
}


void ACTF_CapturePoint::ServerRespawn_Implementation()
{
	Respawn();
}

bool ACTF_CapturePoint::ServerRespawn_Validate()
{
	return true;
}

void ACTF_CapturePoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTF_CapturePoint, FlagInstance);
}
