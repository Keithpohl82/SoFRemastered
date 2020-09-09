// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "HealthComponent.h"

#include "HitScanWeapon.h"
#include "MasterGameMode.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "SOFPlayerController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "../SoFRemastered.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "SPlayerState.h"
#include "SOFGameState.h"
#include "ShooterCameraManager.h"





 FOnMasterCharacterEquipWeapon AMasterCharacter::NotifyEquipWeapon;
 FOnMasterCharacteUnEquipWeapon AMasterCharacter::NotifyUnEquipWeapon;


// Sets default values
AMasterCharacter::AMasterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(CameraComp);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);


	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	FlagAttachSocketName = "FlagSocket";//socket where flag will attach

	bWantsToFire = false;
	bIsTargeting = false;
	
}





// Called when the game starts or when spawned
void AMasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AMasterCharacter::OnHealthChanged);

}



FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}



// Called every frame
void AMasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	ASPlayerState* PState = Cast<ASPlayerState>(GetPlayerState());
	if (PState)
	{
		TeamNum = PState->TeamNum;
		//UE_LOG(LogTemp, Warning, TEXT("SCharacter Team: %d"), TeamNum);
	}
}

//Move Forward and Backward
void AMasterCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

//Move Left and Right
void AMasterCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

//Starts Crouch. Hold to keep crouch
void AMasterCharacter::BeginCrouch()
{
	Crouch();
}

//Stops Crouch. Release to stand up
void AMasterCharacter::EndCrouch()
{
	UnCrouch();
}



void AMasterCharacter::OnStartFire()
{
	ASOFPlayerController* MyPC = Cast<ASOFPlayerController>(Controller);
	if (MyPC )
	{
		StartWeaponFire();
	}
}

void AMasterCharacter::OnStopFire()
{
	StopWeaponFire();
}

void AMasterCharacter::OnNextWeapon()
{
	ASOFPlayerController* MyPC = Cast<ASOFPlayerController>(Controller);
	if (MyPC )
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AHitScanWeapon* NextWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(NextWeapon);
		}
	}
}

void AMasterCharacter::OnPrevWeapon()
{
	ASOFPlayerController* MyPC = Cast<ASOFPlayerController>(Controller);
	if (MyPC)
	{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AHitScanWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
			EquipWeapon(PrevWeapon);
		}
	}
}

void AMasterCharacter::OnReload()
{
	ASOFPlayerController* MyPC = Cast<ASOFPlayerController>(Controller);
	if (MyPC)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
		}
	}
}

class AHitScanWeapon* AMasterCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

bool AMasterCharacter::CanFire() const
{
	return IsAlive();
}

bool AMasterCharacter::CanReload() const
{
	return true;
}

bool AMasterCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

void AMasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorldTimerManager().SetTimerForNextTick(this, &AMasterCharacter::SpawnDefaultInventory);
}

void AMasterCharacter::EquipWeapon(class AHitScanWeapon* Weapon)
{
	if (Weapon)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}



void AMasterCharacter::ServerEquipWeapon_Implementation(class AHitScanWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool AMasterCharacter::ServerEquipWeapon_Validate(class AHitScanWeapon* NewWeapon)
{
	return true;
}



bool AMasterCharacter::IsAlive() const
{
	return HealthComp->Health > 0;
}

bool AMasterCharacter::IsTargeting() const
{
	return bIsTargeting;
}

void AMasterCharacter::SetCurrentWeapon(class AHitScanWeapon* NewWeapon, class AHitScanWeapon* LastWeapon /*= NULL*/)
{
	AHitScanWeapon* LocalLastWeapon = nullptr;
	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
	LocalLastWeapon = CurrentWeapon;
	}

	//UnEquip previous weapon
	if (LocalLastWeapon)
	{
	LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	//Equip a new weapon
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);

		NewWeapon->OnEquip(LastWeapon);
	}
}

void AMasterCharacter::OnRep_CurrentWeapon(class AHitScanWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AMasterCharacter::Destroyed()
{
	Super::Destroyed();
	DestroyInventory();
}

int32 AMasterCharacter::GetInventoryCount() const
{
	return Inventory.Num();
}

class AHitScanWeapon* AMasterCharacter::GetInventoryWeapon(int32 index) const
{
	return Inventory[index];
}

void AMasterCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}

FRotator AMasterCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

bool AMasterCharacter::IsEnemyFor(AController* TestPC) const
{
	if (TestPC == Controller || TestPC == NULL)
	{
		return false;
	}

	ASPlayerState* TestPlayerState = Cast<ASPlayerState>(TestPC->PlayerState);
	ASPlayerState* MyPlayerState = Cast<ASPlayerState>(GetPlayerState());

	bool bIsEnemy = true;
	if (GetWorld()->GetGameState())
	{
		const AMasterGameMode* DefGame = GetWorld()->GetGameState()->GetDefaultGameMode<AMasterGameMode>();
		if (DefGame && MyPlayerState && TestPlayerState)
		{
			bIsEnemy = DefGame->CanDealDamage(TestPlayerState, MyPlayerState);
		}
	}

	return bIsEnemy;
}

void AMasterCharacter::AddWeapon(class AHitScanWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AMasterCharacter::RemoveWeapon(class AHitScanWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

class AHitScanWeapon* AMasterCharacter::FindWeapon(TSubclassOf<class AHitScanWeapon> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return NULL;
}

void AMasterCharacter::SpawnDefaultInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClass = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClass; i++)
	{
		if (DefaultInventoryClasses[i])
		{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AHitScanWeapon* NewWeapon = GetWorld()->SpawnActor<AHitScanWeapon>(DefaultInventoryClasses[i], SpawnInfo);
		AddWeapon(NewWeapon);
		}
	}
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void AMasterCharacter::DestroyInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AHitScanWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void AMasterCharacter::StartWeaponFire()
{
	
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
			
		}
	}
}

void AMasterCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

USkeletalMeshComponent* AMasterCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}

USkeletalMeshComponent* AMasterCharacter::GetSpecificPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1P : GetMesh();
}

FName AMasterCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}



void AMasterCharacter::OnHealthChanged(UHealthComponent* InHealthComp, float Health, float Armor, float HealthChanged, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCause)
{
	if (Health <= 0.0f && !bPlayerDied)
	{
		bPlayerDied = true;
		
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetLifeSpan(3.0f);

		Deaths++;

		AMasterGameMode* GM = Cast<AMasterGameMode>(GetWorld()->GetAuthGameMode());
		GM->PlayerRespawn();

		DetachFromControllerPendingDestroy();
	}
}

// Called to bind functionality to input
void AMasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMasterCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AMasterCharacter::BeginCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &AMasterCharacter::EndCrouch);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AMasterCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AMasterCharacter::OnPrevWeapon);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMasterCharacter::OnStartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMasterCharacter::OnStopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMasterCharacter::OnReload);
	


}

void AMasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMasterCharacter, bPlayerDied);
	
	DOREPLIFETIME(AMasterCharacter, TeamNum);

	DOREPLIFETIME(AMasterCharacter, CurrentWeapon);

	DOREPLIFETIME(AMasterCharacter, bIsTargeting);


	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AMasterCharacter, Inventory, COND_OwnerOnly);
}