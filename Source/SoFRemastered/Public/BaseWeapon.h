//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "Curves/CurveFloat.h"
//#include "BaseWeapon.generated.h"
//
//class USkeletalMeshComponent;
//class UCurveFloat;
//class UDamageType;
//class UParticleSystem;
//class AMasterCharacter;
//
//
//
//
//namespace EWeaponState
//{
//	enum Type
//	{
//		Idle,
//		Firing,
//		Reloading,
//		Equipping,
//	};
//}
//
//
//USTRUCT()
//struct FBaseHitScanTrace
//{
//	GENERATED_BODY()
//
//public:
//	UPROPERTY()
//	TEnumAsByte<EPhysicalSurface> SurfaceType;
//	UPROPERTY()
//	FVector_NetQuantize TraceTo;
//};
//
//
//USTRUCT()
//struct FBaseWeaponData
//{
//	GENERATED_USTRUCT_BODY()
//
//	//Max ammo amount allowed
//	UPROPERTY(EditDefaultsOnly, Category = Ammo)
//	int32 MaxAmmo;
//
//	//Amount of ammo in a clip
//	UPROPERTY(EditDefaultsOnly, Category = Ammo)
//	int32 AmmoPerClip;
//
//	//Starting number of clips
//	UPROPERTY(EditDefaultsOnly, Category = Ammo)
//	int32 InitialClips;
//
//	//Time between shots for Rate of fire
//	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
//	float TimeBetweenShots;
//
//	//Timer for reloading weapon 
//	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
//	float ReloadDuration;
//
//	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
//	float RecoilTimeBetweenShots;
//
//
//	//SetsDefaults
//	FBaseWeaponData()
//	{
//		MaxAmmo = 90;
//		AmmoPerClip = 25;
//		InitialClips = 3;
//		TimeBetweenShots = 0.2f;
//		ReloadDuration = 1.0f;
//		RecoilTimeBetweenShots = 0.0f;
//	}
//
//};
//
//UCLASS(Abstract, Blueprintable)
//class SOFREMASTERED_API ABaseWeapon : public AActor
//{
//	GENERATED_UCLASS_BODY()
//
//
//	virtual void PostInitializeComponents() override;
//		
//
//	//////////////////////////////////////////////////////////////////////////
//	//Ammo
//
//	enum class EAmmoType
//	{
//		EBullet,
//		ERocket,
//		EMax
//	};
//
//	/* Server Add Ammo */
//	void GiveAmmo(int AddAmount);
//
//	/* Uses a bullet */
//	void UseAmmo();
//
//	virtual EAmmoType GetAmmoType() const
//	{
//		return EAmmoType::EBullet;
//	}
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//Control
//
//	/* check if weapon can fire */
//	bool CanFire() const;
//
//	/* check if weapon can be reloaded */
//	bool CanReload() const;
//
//	//////////////////////////////////////////////////////////////////////////
//	//Reading Data
//
//	/* Getter for Getting Max ammo*/
//	int32 GetMaxAmmo() const;
//
//	/* Getter for Ammo per clip */
//	int32 GetAmmoPerClip() const;
//
//	/* Gets the total ammo amount*/
//	int32 GetCurrentAmmo() const;
//
//	int32 GetCurrentAmmoInClip() const;
//
//	/* Sets who the owner of the weapon is */
//	void SetOwningPawn(class AMasterCharacter* AMasterCharacter);
//
//	UPROPERTY(EditDefaultsOnly, Category = WeaponStats)
//	float RecoilTimePerShot;
//
//	/* Getter for the equip start time */
//	float GetEquipStartedTime() const;
//
//	/* Getter for the equip duration */
//	float GetEquipDruation() const;
//
//	/* Getter for the current weapon state */
//	EWeaponState::Type GetCurrentState() const;
//
//	/* Determines if 3rd person or 1st person mesh to spawn */
//	USkeletalMeshComponent* GetWeaponMesh() const;
//
//	/* Determines which player to spawn weapon*/
//	class AMasterCharacter* GetPawnOwner() const;
//
//	/** Adjustment to handle frame rate affecting actual timer interval. */
//	UPROPERTY(Transient)
//	float TimerIntervalAdjustment;
//
//	/** Whether to allow automatic weapons to catch up with shorter refire cycles */
//	UPROPERTY(Config)
//	bool bAllowAutomaticWeaponCatchup = true;
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//Inventory
//
//	/** weapon is being equipped by owner pawn */
//	virtual void OnEquip(const ABaseWeapon* LastWeapon);
//
//	/** weapon is now equipped by owner pawn */
//	virtual void OnEquipFinished();
//
//	/** weapon is holstered by owner pawn */
//	virtual void OnUnEquip();
//
//	/** [server] weapon was added to pawn's inventory */
//	virtual void OnEnterInventory(AMasterCharacter* NewOwner);
//
//	/** [server] weapon was removed from pawn's inventory */
//	virtual void OnLeaveInventory();
//
//	/** check if it's currently equipped */
//	bool IsEquipped() const;
//
//	/** check if mesh is already attached */
//	bool IsAttachedToPawn() const;
//
//
//	//////////////////////////////////////////////////////////////////////////
//	//Inputs
//
//	/* [local + server] start weapon fire */
//	virtual void StartFire();
//
//	/* [local + server] stop weapon fire */
//	virtual void StopFire();
//
//	/* [all] start weapon reload */
//	virtual void StartReload(bool bFromReplication = false);
//
//	/* [local + server] interrupt weapon reload */
//	virtual void StopReload();
//
//	/* [server] performs actual reload */
//	virtual void ReloadWeapon();
//
//	/* Calls the reload from the server */
//	UFUNCTION(reliable, client)
//	void ClientStartReload();
//
//
////////////////////////////////////////////////////////////////////////////////
//// Server Side Inputs
//
//	UFUNCTION(reliable, server, WithValidation)
//	void ServerStartFire();
//
//	UFUNCTION(reliable, server, WithValidation)
//	void ServerStopFire();
//
//	UFUNCTION(reliable, server, WithValidation)
//	void ServerStartReload();
//
//	UFUNCTION(reliable, server, WithValidation)
//	void ServerStopReload();
//
//
////////////////////////////////////////////////////////////////////////////////
//// For Replication
//
//	UFUNCTION()
//	void OnRep_MyPawn();
//
//	UFUNCTION()
//	void OnRep_Reload();
//
//	UFUNCTION()
//	void OnRep_BurstCounter();
//
//	virtual void SimulateWeaponFire();
//
//	virtual void StopSimulatingWeaponFire();
//
//
////////////////////////////////////////////////////////////////////////////////
////  Weapon Specific
//
//	/* For Use by child classes??? */
//	virtual void FireWeapon() PURE_VIRTUAL(ABaseWeapon::FireWeapon, );
//
//	UFUNCTION(reliable, server, WithValidation)
//	void ServerHandleFiring();
//
//	void HandleFiring();
//
//	void HandleReFiring();
//
//	virtual void OnBurstStarted(); 
//
//	virtual void OnBurstFinished();
//
//	/* Updates the weapon's state */
//	void SetWeaponState(EWeaponState::Type NewState);
//
//	/* Determines what state the weapon is in, See Weapon state enum*/
//	void DeterMineWeaponState();
//
//
////////////////////////////////////////////////////////////////////////////
////Inventory
//
//	/* Attaches weapon mesh to the player pawn*/
//	void AttachMeshToPawn();
//
//	/* Detaches weapon mesh to the player pawn*/
//	void DetachMeshFromPawn();
//
//
////////////////////////////////////////////////////////////////////////////
////Weapon firing helpers
//
///* Get the aim of the weapon, allowing for adjustments to be made by the weapon */
//	virtual FVector GetAdjustedAim() const;
//
//	/* Get the aim of the camera */
//	FVector GetCameraAim() const;
//
//	/* get the originating location for camera damage */
//	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;
//
//	/* get the muzzle location of the weapon */
//	FVector GetMuzzleLocation() const;
//
//	/* get direction of weapon's muzzle */
//	FVector GetMuzzleDirection() const;
//
//	/* find hit */
//	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
//
//	/** name of bone/socket for muzzle in weapon mesh */
//	UPROPERTY(EditDefaultsOnly, Category = Effects)
//	FName MuzzleAttachPoint;
//
//
//public:	
//	// Sets default values for this actor's properties
//	ABaseWeapon();
//
//
//protected:
//	/** weapon data */
//	UPROPERTY(EditDefaultsOnly, Category = Config)
//	FBaseWeaponData WeaponConfig;
//
//	/** pawn owner */
//	UPROPERTY(Transient, ReplicatedUsing=OnRep_MyPawn)
//	class AMasterCharacter* MyPawn;
//
//private:
//	UPROPERTY(EditAnywhere, Category = Mesh)
//	USkeletalMeshComponent* GunMesh1P;
//
//	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
//	USkeletalMeshComponent* GunMesh3P;
//
//protected:
//
//	void Recoil();
//
//	void ResetRecoil();
//	
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
//	TSubclassOf<UDamageType> DamageType;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
//	UCurveFloat* VerticalRecoil;
//	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
//	UCurveFloat* HorizontalRecoil;
//	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
//	float RecoilRecoveryTime;
//	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
//	float RecoilRecoveryDelay;
//	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
//	float RecoilMultiplyer;
//
//	AMasterCharacter* PlayerCharacter;
//	
//	/* is weapon currently equipped? */
//	uint32 bIsEquipped : 1;
//
//	/* is weapon fire active? */
//	uint32 bWantsToFire : 1;
//
//	/* is weapon being equipped? */
//	uint32 bPendingEquip : 1;
//
//	/*is weapon reloading? ;*/
//	UPROPERTY(Transient, ReplicatedUsing=OnRep_Reload)
//	uint32 bPendingReload : 1;
//
//	/** weapon is refiring */
//	uint32 bRefiring;
//
//	/* Current Weapon State */
//	EWeaponState::Type CurrentState;
//
//	/* Last time weapon was fired */
//	float LastFireTime;
//
//	/* When the Weapon was switched to*/
//	float EquipStartedTime;
//
//	/* How much time the weapon needs to be equipped*/
//	float EquipDuration;
//
//	/* Current total ammo */
//	UPROPERTY(transient, Replicated)
//	int32 CurrentAmmo;
//
//	/* Current ammo - inside clip*/
//	UPROPERTY(Transient, Replicated)
//	int32  CurrentAmmoInClip;
//
//	/* Handle for OnEquipFinished timer */
//	FTimerHandle TimerHandle_OnEquipFinished;
//
//	/* Handle for StopReload timer */
//	FTimerHandle TimerHandle_StopReload;
//
//	/* Handle for ReloadWeapon timer */
//	FTimerHandle TimerHandle_ReloadWeapon;
//
//	/* Handle for HandleFiring timer */
//	FTimerHandle TimerHandle_HandleFiring;
//
//	/* burst counter, used for replicating fire events to remote clients */
//	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
//	int32 BurstCounter;
//
//
//	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const {return GunMesh1P;}
//	FORCEINLINE USkeletalMeshComponent* GetMesh3P() const {return GunMesh3P;}
//
//
//};
//
//
