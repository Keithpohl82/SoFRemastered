// Fill out your copyright notice in the Description page of Project Settings.

#include "HitScanWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MasterCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "../SoFRemastered.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"




static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDbugWeaponDrawing(TEXT("DrawDebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for weapons"),
	ECVF_Cheat);


// Sets default values
AHitScanWeapon::AHitScanWeapon()
{

GunMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("1st Person Gun Mesh"));
GunMesh1P->SetOnlyOwnerSee(true);
GunMesh1P->SetOnlyOwnerSee(false);
GunMesh1P->bReceivesDecals = false;
GunMesh1P->CastShadow = false;
GunMesh1P->SetCollisionObjectType(ECC_WorldDynamic);
GunMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
GunMesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
RootComponent = GunMesh1P;

GunMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("3rd Person Weapon Mesh"));
GunMesh3P->SetOwnerNoSee(true);
GunMesh3P->SetOnlyOwnerSee(false);
GunMesh3P->bReceivesDecals = false;
GunMesh3P->CastShadow = true;
GunMesh3P->SetCollisionObjectType(ECC_WorldDynamic);
GunMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
GunMesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
GunMesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
GunMesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
GunMesh3P->SetupAttachment(GunMesh1P);


	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	EjectSocketName = "AmmoEject";

	// Sets default values
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void AHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShots = 60 / WeaponConfig.RateOfFire;

	Ammo = WeaponConfig.AmmoPerClip;

}

EWeaponState::Type AHitScanWeapon::GetCurrentState() const
{
	return CurrentState;
}

void AHitScanWeapon::SetOwningPawn(class AMasterCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;

		SetOwner(NewOwner);
	}
}

void AHitScanWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}


void AHitScanWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;
	CurrentState = NewState;
}

bool AHitScanWeapon::CanFire() const
{
	bool bCanFire = MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool AHitScanWeapon::CanReload() const
{
	bool bCanReload = (!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0);
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}

USkeletalMeshComponent* AHitScanWeapon::GetWeaponMesh() const
{
	return (MyPawn != NULL && MyPawn->IsFirstPerson() ? GunMesh1P : GunMesh3P);
}

void AHitScanWeapon::PlayFireEffects(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh1P, MuzzleSocketName);
		UGameplayStatics::SpawnEmitterAttached(EjectEffect, GunMesh1P, EjectSocketName);
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh1P, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = GunMesh1P->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
	Recoil();
}

void AHitScanWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = GunMesh1P->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, ImpactPoint, ShotDirection.Rotation());

	}
}

void AHitScanWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

	if (Ammo <= 0)
	{
		UGameplayStatics::SpawnSoundAttached(OutOfAmmoSound, GunMesh1P, MuzzleSocketName);

	}

	if (Ammo > 0)
	{

		if (bIsShotGun)
		{
			for (int32 i = 0; i <10; i++) 
			{
				Ammo--;
				

				AActor* MyOwner = GetOwner();

				if (MyOwner)
				{
					FVector EyeLocation;
					FRotator EyeRotation;

					APawn* PawnOwner = Cast<APawn>(GetOwner());
					AController* ControllerOwner = PawnOwner->GetController();
					ControllerOwner->GetPlayerViewPoint(EyeLocation, EyeRotation);

					float SpreadMax = 1.1;
					float SpreadMin = -1.1;
					FVector ShotDirection = FMath::VRandCone(EyeRotation.Vector(), 0.07);

					FVector TraceEnd = EyeLocation + (ShotDirection * WeaponConfig.BulletEndPoint);

					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(MyOwner);
					QueryParams.AddIgnoredActor(this);

					QueryParams.bTraceComplex = true;
					QueryParams.bReturnPhysicalMaterial = true;

					FVector TracerEndPoint = TraceEnd;

					EPhysicalSurface SurfaceType = SurfaceType_Default;

					FHitResult Hit;


					if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
					{
						AActor* HitActor = Hit.GetActor();

						SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

						float ActualDamage = BaseDamage;
						if (SurfaceType == SURFACE_FLESHVULNERABLE)
						{
							ActualDamage *= HeadShotBonus;
						}

						UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

						PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

						TracerEndPoint = Hit.ImpactPoint;
					}

					if (DebugWeaponDrawing > 0)
					{
						DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red,false, 10.0f, 0, 1.0f);
					}

					PlayFireEffects(TracerEndPoint);

					if (GetLocalRole() == ROLE_Authority)
					{
						HitScanTrace.TraceTo = TracerEndPoint;
						HitScanTrace.SurfaceType = SurfaceType;
					}

					LastFireTime = GetWorld()->TimeSeconds;
				}
			}
		}
		else
		{ 
		Ammo--;

		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			FVector EyeLocation;
			FRotator EyeRotation;

			APawn* PawnOwner = Cast<APawn>(GetOwner());
			AController* ControllerOwner = PawnOwner->GetController();
			ControllerOwner->GetPlayerViewPoint(EyeLocation, EyeRotation);

			FVector ShotDirection = EyeRotation.Vector();

			FVector TraceEnd = EyeLocation + (ShotDirection * WeaponConfig.BulletEndPoint);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);

			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			FVector TracerEndPoint = TraceEnd;

			EPhysicalSurface SurfaceType = SurfaceType_Default;

			FHitResult Hit;

			
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{
				AActor* HitActor = Hit.GetActor();

				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE)
				{
					ActualDamage *= HeadShotBonus;
				}

				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

				PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

				TracerEndPoint = Hit.ImpactPoint;
			}

			if (DebugWeaponDrawing > 0)
			{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Green, false, 10.0f, 0, 1.0f);
			}

			PlayFireEffects(TracerEndPoint);

			if (GetLocalRole() == ROLE_Authority)
			{
				HitScanTrace.TraceTo = TracerEndPoint;
				HitScanTrace.SurfaceType = SurfaceType;
			}

			LastFireTime = GetWorld()->TimeSeconds;
		}
		}
	}
}

void AHitScanWeapon::Recoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = RecoilTimePerShot + TimeBetweenShots;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));

		}

	}
}

void AHitScanWeapon::ResetRecoil()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
			FRotator PitchRotation = PC->GetControlRotation();
			RecoilTimePerShot = 0;
			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot);
			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot);
			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
		}
	}
}

void AHitScanWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		DetachMeshFromPawn();

		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecificPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecificPawnMesh(false);
			GunMesh1P->SetHiddenInGame(false);
			GunMesh3P->SetHiddenInGame(false);
			GunMesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			GunMesh3P->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AHitScanWeapon::DetachMeshFromPawn()
{
	GunMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	GunMesh1P->SetHiddenInGame(true);

	GunMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	GunMesh3P->SetHiddenInGame(true);
}

void AHitScanWeapon::DeterMineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}
	SetWeaponState(NewState);
}

void AHitScanWeapon::OnEquip(const AHitScanWeapon* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DeterMineWeaponState();

	if (LastWeapon)
	{
		float Duration = 0.5f;
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AHitScanWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}
	AMasterCharacter::NotifyEquipWeapon.Broadcast(MyPawn, this);
}

void AHitScanWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	DeterMineWeaponState();

	if (MyPawn)
	{
		if (MyPawn->IsLocallyControlled() && CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}
	}
}

void AHitScanWeapon::OnUnEquip()
{
	DetachMeshFromPawn();
	bIsEquipped = false;
	StopFire();

	if (bPendingReload)
	{
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}
	AMasterCharacter::NotifyUnEquipWeapon.Broadcast(MyPawn, this);
}

void AHitScanWeapon::OnEnterInventory(AMasterCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void AHitScanWeapon::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
}

bool AHitScanWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool AHitScanWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}


void AHitScanWeapon::StartFire()
{
	if (isAuto)
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AHitScanWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	}
	else
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AHitScanWeapon::Fire, TimeBetweenShots, false, FirstDelay);

	}
}

void AHitScanWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	ResetRecoil();
}

void AHitScanWeapon::ServerReloadGun_Implementation()
{
	StartReload(true);
}

bool AHitScanWeapon::ServerReloadGun_Validate()
{
	return true;
}

void AHitScanWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{
		
		StartReload(true);
		
	}
	else
	{
		StopReload();
	}
}

void AHitScanWeapon::Reload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerReloadGun();
	}

	if (WeaponConfig.MaxAmmo < WeaponConfig.AmmoPerClip)
	{
		AmountToReload = WeaponConfig.MaxAmmo;

		WeaponConfig.MaxAmmo = 0;

		Ammo = Ammo + AmountToReload;

		
	}
	else
	{
		AmountToReload = WeaponConfig.AmmoPerClip - Ammo;

		WeaponConfig.MaxAmmo = WeaponConfig.MaxAmmo - AmountToReload;

		Ammo = WeaponConfig.AmmoPerClip;

	}
}

void AHitScanWeapon::StartReload(bool bFromReplication /*= false*/)
{
	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerReloadGun();
	}

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DeterMineWeaponState();
		
		float Duration = 5.5f;
		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AHitScanWeapon::StopReload, WeaponConfig.ReloadDuration, false);
		UE_LOG(LogTemp, Warning, TEXT("STOP  Reload timer should be called here......"))
			if (GetLocalRole() == ROLE_Authority)
			{
				UE_LOG(LogTemp, Warning, TEXT("Reload timer should be called here......"))
				GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AHitScanWeapon::Reload, FMath::Max(0.1f, WeaponConfig.ReloadDuration - 0.1f), false);
			}

	}
}

void AHitScanWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DeterMineWeaponState();
	}
}

int AHitScanWeapon::GetAmmo()
{
	return Ammo;
}

int AHitScanWeapon::GetTotalAmmo()
{
	return WeaponConfig.MaxAmmo;
}

void AHitScanWeapon::ServerFire_Implementation()
{
	Fire();
}

bool AHitScanWeapon::ServerFire_Validate()
{
	return true;
}

void AHitScanWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}



void AHitScanWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHitScanWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(AHitScanWeapon, CurrentAmmoInClip);
	DOREPLIFETIME(AHitScanWeapon, CurrentAmmo);
	DOREPLIFETIME(AHitScanWeapon, AmountToReload);
	DOREPLIFETIME(AHitScanWeapon, Ammo);
}

