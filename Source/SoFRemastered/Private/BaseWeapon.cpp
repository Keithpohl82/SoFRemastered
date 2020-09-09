//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BaseWeapon.h"
//#include "Components/SkeletalMeshComponent.h"
//#include "MasterCharacter.h"
//#include "DrawDebugHelpers.h"
//#include "Kismet/GameplayStatics.h"
//#include "Particles/ParticleSystem.h"
//#include "Particles/ParticleSystemComponent.h"
//#include "TimerManager.h"
//#include "../SoFRemastered.h"
//#include "PhysicalMaterials/PhysicalMaterial.h"
//#include "Net/UnrealNetwork.h"
//#include "SPlayerState.h"
//#include "SOFPlayerController.h"
//
//
//
//
////static int32 DebugWeaponDrawing = 0;
////FAutoConsoleVariableRef CVARDbugWeaponDrawing(TEXT("DrawDebugWeapons"), 
////	DebugWeaponDrawing, 
////	TEXT("Draw Debug Lines for weapons"), 
////		ECVF_Cheat);
//
//
//
//
//ABaseWeapon::ABaseWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
//{
//	GunMesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("1st Person Gun Mesh"));
//	GunMesh1P->SetOnlyOwnerSee(true);
//	GunMesh1P->SetOnlyOwnerSee(false);
//	GunMesh1P->bReceivesDecals = false;
//	GunMesh1P->CastShadow = false;
//	GunMesh1P->SetCollisionObjectType(ECC_WorldDynamic);
//	GunMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	GunMesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
//	RootComponent = GunMesh1P;
//
//	GunMesh3P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("3rd Person Weapon Mesh"));
//	GunMesh3P->SetOwnerNoSee(true);
//	GunMesh3P->SetOnlyOwnerSee(false);
//	GunMesh3P->bReceivesDecals = false;
//	GunMesh3P->CastShadow = true;
//	GunMesh3P->SetCollisionObjectType(ECC_WorldDynamic);
//	GunMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	GunMesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
//	GunMesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
//	GunMesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
//	GunMesh3P->SetupAttachment(GunMesh1P);
//	
//	// Sets default values
//	bIsEquipped = false;
//	bWantsToFire = false;
//	bPendingReload = false; 
//	bPendingEquip = false;
//	CurrentState = EWeaponState::Idle;
//
//	CurrentAmmo = 0;
//	CurrentAmmoInClip = 0;
//	LastFireTime = 0.0f;
//
//	bReplicates = true;
//	bNetUseOwnerRelevancy = true;
//
//	NetUpdateFrequency = 66.0f;
//	MinNetUpdateFrequency = 33.0f;
//
//}
//
//void ABaseWeapon::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//
//	if (WeaponConfig.InitialClips > 0)
//	{
//		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
//		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
//	}
//}
//
//void ABaseWeapon::GiveAmmo(int AddAmount)
//{
//	// Gets the amount of missing Ammo
//	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
//	// Gets the amount of ammo to give
//	AddAmount = FMath::Min(AddAmount, MissingAmmo);
//	// Sets the amount of ammo
//	CurrentAmmo += AddAmount;
//
//	
//
//	// Starts the Reload if clip is empty
//	if (GetCurrentAmmoInClip() <= 0 && CanReload() && MyPawn && (MyPawn->GetWeapon() == this))
//	{
//		ClientStartReload();
//	}
//
//}
//
//void ABaseWeapon::UseAmmo()
//{
//	CurrentAmmoInClip--;
//
//	CurrentAmmo--;
//
//	
//}
//
//bool ABaseWeapon::CanFire() const
//{
//	bool bCanFire = MyPawn && MyPawn->CanFire();
//	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
//	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
//}
//
//bool ABaseWeapon::CanReload() const
//{
//	bool bCanReload = (!MyPawn || MyPawn->CanReload());
//	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0);
//	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
//	return((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
//
//}
//
//int32 ABaseWeapon::GetMaxAmmo() const
//{
//	return WeaponConfig.MaxAmmo;
//}
//
//int32 ABaseWeapon::GetAmmoPerClip() const
//{
//	return WeaponConfig.AmmoPerClip;
//}
//
//int32 ABaseWeapon::GetCurrentAmmo() const
//{
//	return CurrentAmmo;
//}
//
//int32 ABaseWeapon::GetCurrentAmmoInClip() const
//{
//	return CurrentAmmoInClip;
//}
//
//void ABaseWeapon::SetOwningPawn(AMasterCharacter* NewOwner)
//{
//	if (MyPawn != NewOwner)
//	{
//		SetInstigator(NewOwner);
//		MyPawn = NewOwner;
//
//		SetOwner(NewOwner);
//	}
//}
//
//float ABaseWeapon::GetEquipStartedTime() const
//{
//	return EquipStartedTime;
//}
//
//float ABaseWeapon::GetEquipDruation() const
//{
//	return EquipDuration;
//}
//
//EWeaponState::Type ABaseWeapon::GetCurrentState() const
//{
//	return CurrentState;
//}
//
//USkeletalMeshComponent* ABaseWeapon::GetWeaponMesh() const
//{
//	return (MyPawn != NULL && MyPawn->IsFirstPerson() ? GunMesh1P : GunMesh3P);
//}
//
//class AMasterCharacter* ABaseWeapon::GetPawnOwner() const
//{
//	return MyPawn;
//}
//
//void ABaseWeapon::OnEquip(const ABaseWeapon* LastWeapon)
//{
//	AttachMeshToPawn();
//
//	bPendingEquip = true;
//	DeterMineWeaponState();
//	
//	if (LastWeapon)
//	{
//		float Duration = 0.5f;
//		EquipStartedTime = GetWorld()->GetTimeSeconds();
//		EquipDuration = Duration;
//
//		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &ABaseWeapon::OnEquipFinished, Duration, false);
//	}
//	else
//	{
//		OnEquipFinished();
//	}
//	AMasterCharacter::NotifyEquipWeapon.Broadcast(MyPawn, this);
//}
//
//void ABaseWeapon::OnEquipFinished()
//{
//	AttachMeshToPawn();
//
//	bIsEquipped = true;
//	bPendingEquip = false;
//
//	DeterMineWeaponState();
//
//	if (MyPawn)
//	{
//		if (MyPawn->IsLocallyControlled() && CurrentAmmoInClip <= 0 && CanReload())
//		{
//			StartReload();
//		}
//	}
//}
//
//void ABaseWeapon::OnUnEquip()
//{
//	DetachMeshFromPawn();
//	bIsEquipped = false;
//	StopFire();
//
//	if (bPendingReload)
//	{
//		bPendingReload = false;
//
//		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
//		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
//	}
//
//	if (bPendingEquip)
//	{
//		bPendingEquip = false;
//		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
//	}
//	AMasterCharacter::NotifyUnEquipWeapon.Broadcast(MyPawn, this);
//}
//
//void ABaseWeapon::OnEnterInventory(AMasterCharacter* NewOwner)
//{
//	SetOwningPawn(NewOwner);
//}
//
//void ABaseWeapon::OnLeaveInventory()
//{
//	if (IsAttachedToPawn())
//	{
//		OnUnEquip();
//	}
//
//	if (GetLocalRole() == ROLE_Authority)
//	{
//		SetOwningPawn(NULL);
//	}
//}
//
//bool ABaseWeapon::IsEquipped() const
//{
//	return bIsEquipped;
//}
//
//bool ABaseWeapon::IsAttachedToPawn() const
//{
//	return bIsEquipped || bPendingEquip;
//}
//
//void ABaseWeapon::StartFire()
//{
//	
//	if (GetLocalRole() < ROLE_Authority)
//	{
//		ServerStartFire();
//	}
//	if (bWantsToFire)
//	{
//		bWantsToFire = true;
//		DeterMineWeaponState();
//	}
//}
//
//void ABaseWeapon::StopFire()
//{
//	if ((GetLocalRole() < ROLE_Authority) && MyPawn && MyPawn->IsLocallyControlled())
//	{
//		ServerStopFire();
//	}
//	if (bWantsToFire)
//	{
//		bWantsToFire = false;
//		DeterMineWeaponState();
//
//	}
//}
//
//
//
//void ABaseWeapon::StartReload(bool bFromReplication)
//{
//	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
//	{
//
//	//SERVERRELOADGUN***********
//		ServerStartReload();
//	}
//
//	if (bFromReplication || CanReload())
//	{
//		bPendingReload = true;
//		DeterMineWeaponState();
//
//		float Duration = 0.5f;
//		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseWeapon::StopReload, Duration, false);
//
//			if (GetLocalRole() == ROLE_Authority)
//			{
//				GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseWeapon::ReloadWeapon, FMath::Max(0.1f, Duration -0.1f), false);
//			}
//
//	}
//}
//
//void ABaseWeapon::StopReload()
//{
//	if (CurrentState == EWeaponState::Reloading)
//	{
//		bPendingReload = false;
//		DeterMineWeaponState();
//	}
//}
//
//void ABaseWeapon::ReloadWeapon()
//{
//
//		//Reload()************
//	int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);
//
//	if (ClipDelta > 0)
//	{
//		CurrentAmmoInClip += ClipDelta;
//	}
//}
//
//void ABaseWeapon::ClientStartReload_Implementation()
//{
//	StartReload();
//}
//
//void ABaseWeapon::ServerStartFire_Implementation()
//{
//	StartFire();
//}
//
//bool ABaseWeapon::ServerStartFire_Validate()
//{
//	return true;
//}
//
//void ABaseWeapon::ServerStopFire_Implementation()
//{
//	StopFire();
//}
//
//bool ABaseWeapon::ServerStopFire_Validate()
//{
//	return true;
//}
//
//void ABaseWeapon::ServerStartReload_Implementation()
//{
//
////STARTRELOAD
//	StartReload();
//}
//
//bool ABaseWeapon::ServerStartReload_Validate()
//{
//	return true;
//}
//
//void ABaseWeapon::ServerStopReload_Implementation()
//{
//	StopReload();
//}
//
//bool ABaseWeapon::ServerStopReload_Validate()
//{
//	return true;
//}
//
//void ABaseWeapon::OnRep_MyPawn()
//{
//	if (MyPawn)
//	{
//		OnEnterInventory(MyPawn);
//	}
//	else
//	{
//		OnLeaveInventory();
//	}
//}
//
//void ABaseWeapon::OnRep_Reload()
//{
//	if (bPendingReload)
//	{
//		StartReload(true);
//	}
//	else
//	{
//		StopReload();
//	}
//}
//
//void ABaseWeapon::OnRep_BurstCounter()
//{
//	if (BurstCounter > 0)
//	{
//		SimulateWeaponFire();
//	}
//	else
//	{
//		StopSimulatingWeaponFire();
//	}
//}
//
//void ABaseWeapon::SimulateWeaponFire()
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("Base Weapon simulateWeaponFire"))
//
//	if (GetLocalRole() == ROLE_Authority && CurrentState != EWeaponState::Firing)
//	{
//		return;
//	}
//
//	//	 if (MuzzleFX)
//	//	{
//	//	USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
//	//	if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
//	//	{
//	//		// Split screen requires we create 2 effects. One that we see and one that the other player sees.
//	//		if( (MyPawn != NULL ) && ( MyPawn->IsLocallyControlled() == true ) )
//	//		{
//	//			AController* PlayerCon = MyPawn->GetController();
//	//			if( PlayerCon != NULL )
//	//			{
//	//				Mesh1P->GetSocketLocation(MuzzleAttachPoint);
//	//				MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
//	//				MuzzlePSC->bOwnerNoSee = false;
//	//				MuzzlePSC->bOnlyOwnerSee = true;
//
//	//				Mesh3P->GetSocketLocation(MuzzleAttachPoint);
//	//				MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh3P, MuzzleAttachPoint);
//	//				MuzzlePSCSecondary->bOwnerNoSee = true;
//	//				MuzzlePSCSecondary->bOnlyOwnerSee = false;
//	//			}
//	//		}
//	//		else
//	//		{
//	//			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
//	//		}
//	//	}
//	//}
//
//	//if (!bLoopedFireAnim || !bPlayingFireAnim)
//	//{
//	//	PlayWeaponAnimation(FireAnim);
//	//	bPlayingFireAnim = true;
//	//}
//
//	//if (bLoopedFireSound)
//	//{
//	//	if (FireAC == NULL)
//	//	{
//	//		FireAC = PlayWeaponSound(FireLoopSound);
//	//	}
//	//}
//	//else
//	//{
//	//	PlayWeaponSound(FireSound);
//	//}
//
//	//AShooterPlayerController* PC = (MyPawn != NULL) ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
//	//if (PC != NULL && PC->IsLocalController())
//	//{
//	//	if (FireCameraShake != NULL)
//	//	{
//	//		PC->ClientPlayCameraShake(FireCameraShake, 1);
//	//	}
//	//	if (FireForceFeedback != NULL && PC->IsVibrationEnabled())
//	//	{
//	//		FForceFeedbackParameters FFParams;
//	//		FFParams.Tag = "Weapon";
//	//		PC->ClientPlayForceFeedback(FireForceFeedback, FFParams);
//	//	}
//	//}
//	
//
//}
//
//void ABaseWeapon::StopSimulatingWeaponFire()
//{
//	
//	/*if (bLoopedMuzzleFX )
//	{
//		if( MuzzlePSC != NULL )
//		{
//			MuzzlePSC->DeactivateSystem();
//			MuzzlePSC = NULL;
//		}
//		if( MuzzlePSCSecondary != NULL )
//		{
//			MuzzlePSCSecondary->DeactivateSystem();
//			MuzzlePSCSecondary = NULL;
//		}
//	}
//
//	if (bLoopedFireAnim && bPlayingFireAnim)
//	{
//		StopWeaponAnimation(FireAnim);
//		bPlayingFireAnim = false;
//	}
//
//	if (FireAC)
//	{
//		FireAC->FadeOut(0.1f, 0.0f);
//		FireAC = NULL;
//
//		PlayWeaponSound(FireFinishSound);
//	}*/
//	
//}
//
//void ABaseWeapon::ServerHandleFiring_Implementation()
//{
//	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());
//
//	HandleFiring();
//
//	if (bShouldUpdateAmmo)
//	{
//	UseAmmo();
//	}
//}
//
//bool ABaseWeapon::ServerHandleFiring_Validate()
//{
//return true;
//}
//
//void ABaseWeapon::HandleFiring()
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("HandleFiring!!!!!!!!!!!!"))
//
//	if (CurrentAmmoInClip > 0 && CanFire())
//	{
//		if (GetNetMode() != NM_DedicatedServer)
//		{
//			SimulateWeaponFire();
//		}
//
//		if (MyPawn && MyPawn->IsLocallyControlled())
//		{
//			FireWeapon();
//			UseAmmo();
//		}
//	}
//	else if (CanReload())
//	{
//		StartReload();
//	}
//	if (MyPawn && MyPawn->IsLocallyControlled())
//	{
//		if (GetLocalRole() < ROLE_Authority)
//		{
//			ServerHandleFiring();
//		}
//
//		if (CurrentAmmoInClip <= 0 && CanReload())
//		{
//			StartReload();
//		}
//
//		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
//		if (bRefiring)
//		{
//			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, FMath::Max<float>(WeaponConfig.TimeBetweenShots + TimerIntervalAdjustment, false));
//			TimerIntervalAdjustment = 0.0f;
//		}
//	}
//	LastFireTime = GetWorld()->GetTimeSeconds();
//}
//
//void ABaseWeapon::HandleReFiring()
//{
//	UWorld* MyWorld = GetWorld();
//
//	float SlackTimeThisFrame = FMath::Max(0.0f, (MyWorld->TimeSeconds - LastFireTime) -WeaponConfig.TimeBetweenShots);
//	if (bAllowAutomaticWeaponCatchup)
//	{
//		TimerIntervalAdjustment -= SlackTimeThisFrame;
//	}
//	HandleFiring();
//}
//
//void ABaseWeapon::OnBurstStarted()
//{
//	const float GameTime = GetWorld()->GetTimeSeconds();
//	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0 && LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
//	{
//		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
//	}
//	else
//	{
//		HandleFiring();
//	}
//}
//
//void ABaseWeapon::OnBurstFinished()
//{
//	BurstCounter = 0;
//
//	StopSimulatingWeaponFire();
//
//	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
//
//	bRefiring = false;
//
//	TimerIntervalAdjustment = 0.0f;
//}
//
//void ABaseWeapon::SetWeaponState(EWeaponState::Type NewState)
//{
//	const EWeaponState::Type PrevState = CurrentState;
//	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
//	{
//		OnBurstFinished();
//	}
//
//	CurrentState = NewState;
//
//	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
//	{
//		OnBurstStarted();
//	}
//}
//
//void ABaseWeapon::DeterMineWeaponState()
//{
//	EWeaponState::Type NewState = EWeaponState::Idle;
//
//	if (bIsEquipped)
//	{
//		if (bPendingReload)
//		{
//			if (CanReload() == false)
//			{
//				NewState = CurrentState;
//			}
//			else
//			{
//				NewState = EWeaponState::Reloading;
//			}
//		}
//		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
//		{
//			NewState = EWeaponState::Firing;
//		}
//	}
//	else if (bPendingEquip)
//	{
//		NewState = EWeaponState::Equipping;
//	}
//	SetWeaponState(NewState);
//}
//
//void ABaseWeapon::AttachMeshToPawn()
//{
//	if (MyPawn)
//	{
//		DetachMeshFromPawn();
//
//		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
//		if (MyPawn->IsLocallyControlled() == true)
//		{
//			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecificPawnMesh(true);
//			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecificPawnMesh(false);
//			GunMesh1P->SetHiddenInGame(false);
//			GunMesh3P->SetHiddenInGame(false);
//			GunMesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
//			GunMesh3P->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
//		}
//		else
//		{
//			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
//			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
//			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
//			UseWeaponMesh->SetHiddenInGame(false);
//		}
//	}
//}
//
//void ABaseWeapon::DetachMeshFromPawn()
//{
//	GunMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//	GunMesh1P->SetHiddenInGame(true);
//
//	GunMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
//	GunMesh3P->SetHiddenInGame(true);
//}
//
//FVector ABaseWeapon::GetAdjustedAim() const
//{
//	ASOFPlayerController* const PC = GetInstigatorController<ASOFPlayerController>();
//	FVector FinalAim = FVector::ZeroVector;
//	if (PC)
//	{
//		FVector CamLoc;
//		FRotator CamRot;
//		PC->GetPlayerViewPoint(CamLoc, CamRot);
//		FinalAim = CamRot.Vector();
//	}
//	else if (GetInstigator())
//	{
//		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
//	}
//	return FinalAim;
//}
//
//FVector ABaseWeapon::GetCameraAim() const
//{
//	ASOFPlayerController* const PC = GetInstigatorController<ASOFPlayerController>();
//	FVector FinalAim = FVector::ZeroVector;
//	if (PC)
//	{
//		FVector CamLoc;
//		FRotator CamRot;
//		PC->GetPlayerViewPoint(CamLoc, CamRot);
//		FinalAim = CamRot.Vector();
//	}
//	else if (GetInstigator())
//	{
//		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
//	}
//	return FinalAim;
//}
//
//FVector ABaseWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
//{
//	ASOFPlayerController* PC = MyPawn ?Cast<ASOFPlayerController>(MyPawn->Controller) : NULL;
//	FVector OutStartTrace = FVector::ZeroVector;
//
//	if (PC)
//	{
//		FRotator UnusedRot;
//		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);
//
//		OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
//	}
//	return OutStartTrace;
//}
//
//FVector ABaseWeapon::GetMuzzleLocation() const
//{
//	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
//	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
//}
//
//FVector ABaseWeapon::GetMuzzleDirection() const
//{
//	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
//	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
//}
//
//FHitResult ABaseWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
//{
//	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
//	TraceParams.bReturnPhysicalMaterial = true;
//
//	FHitResult Hit(ForceInit);
//	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
//
//	return Hit;
//}
//
////Recoil Sets which point on the curve to call. Adjust curves to change recoil
//void ABaseWeapon::Recoil()
//{
//	APawn* MyOwner = Cast<APawn>(GetOwner());
//	if (MyOwner)
//	{
//		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
//		if (PC)
//		{
//			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
//			FRotator PitchRotation = PC->GetControlRotation();
//			RecoilTimePerShot = WeaponConfig.RecoilTimeBetweenShots + WeaponConfig.TimeBetweenShots;
//			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
//			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot) * RecoilMultiplyer;
//			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
//			
//		}
//		
//	}
//	
//}
//
////Resets the recoil to starting point of the curve
//void ABaseWeapon::ResetRecoil()
//{
//	APawn* MyOwner = Cast<APawn>(GetOwner());
//	if (MyOwner)
//	{
//		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
//		if (PC)
//		{
//			//VerticalRecoil->GetFloatValue(RecoilRecoveryDelay);
//			FRotator PitchRotation = PC->GetControlRotation();
//			RecoilTimePerShot = 0;
//			float NewPitch = VerticalRecoil->GetFloatValue(RecoilTimePerShot);
//			float NewYaw = HorizontalRecoil->GetFloatValue(RecoilTimePerShot);
//			PC->SetControlRotation(PitchRotation.Add(NewPitch, NewYaw, 0.0));
//		}
//	}
//}
//
//
//void ABaseWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ABaseWeapon, MyPawn);
//
//	DOREPLIFETIME_CONDITION(ABaseWeapon, CurrentAmmo, COND_OwnerOnly);
//	DOREPLIFETIME_CONDITION(ABaseWeapon, CurrentAmmoInClip, COND_OwnerOnly);
//
//	DOREPLIFETIME_CONDITION(ABaseWeapon, BurstCounter, COND_SkipOwner);
//	DOREPLIFETIME_CONDITION(ABaseWeapon, bPendingReload, COND_SkipOwner);
//}
//
