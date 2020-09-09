// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MasterCharacter.generated.h"



class UCameraComponent;
class ASOFPlayerController;
class AHitScanWeapon;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMasterCharacterEquipWeapon, AMasterCharacter*, AHitScanWeapon* /* new */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMasterCharacteUnEquipWeapon, AMasterCharacter*, AHitScanWeapon* /* old */);

UCLASS()
class SOFREMASTERED_API AMasterCharacter : public ACharacter
{
	GENERATED_BODY()

		/** spawn inventory, setup initial variables */
		virtual void PostInitializeComponents() override;

		UPROPERTY(EditDefaultsOnly, Category = Camera)
	UCameraComponent* CameraComp;

	void EquipWeapon(class AHitScanWeapon* Weapon);

	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class AHitScanWeapon* NewWeapon);


	/* currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentWeapon)
	class AHitScanWeapon* CurrentWeapon;

	/* check if pawn is still alive */
	bool IsAlive() const;



	/** updates current weapon */
	void SetCurrentWeapon(class AHitScanWeapon* NewWeapon, class AHitScanWeapon* LastWeapon = NULL);

	/** current weapon rep handler */
	UFUNCTION()
	void OnRep_CurrentWeapon(class AHitScanWeapon* LastWeapon);

	/** cleanup inventory */
	virtual void Destroyed() override;


	/** get total number of inventory items */
	int32 GetInventoryCount() const;

	/**
	* get weapon from inventory at index. Index validity is not checked.
	*
	* @param Index Inventory index
	*/
	class AHitScanWeapon* GetInventoryWeapon(int32 index) const;
	


	

	/** get aim offsets */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	FRotator GetAimOffsets() const;

	/**
	* Check if pawn is enemy if given controller.
	*
	* @param	TestPC	Controller to check against.
	*/
	bool IsEnemyFor(AController* TestPC) const;

	/**
	* [server] add weapon to inventory
	*
	* @param Weapon	Weapon to add.
	*/
	void AddWeapon(class AHitScanWeapon* Weapon);

	/**
	* [server] remove weapon from inventory
	*
	* @param Weapon	Weapon to remove.
	*/
	void RemoveWeapon(class AHitScanWeapon* Weapon);

	/**
	* Find in inventory
	*
	* @param WeaponClass	Class of weapon to find.
	*/
	class AHitScanWeapon* FindWeapon(TSubclassOf<class AHitScanWeapon> WeaponClass);

	/** default inventory list */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AHitScanWeapon> > DefaultInventoryClasses;

	/** weapons in inventory */
	UPROPERTY(Transient, Replicated)
	TArray<class AHitScanWeapon*> Inventory;

	/** [server] spawns default inventory */
	void SpawnDefaultInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] starts weapon fire */
	void StartWeaponFire();

	/** [local] stops weapon fire */
	void StopWeaponFire();

	/** current targeting state */
	UPROPERTY(Transient, Replicated)
	uint8 bIsTargeting : 1;
	

private:

	UPROPERTY(EditAnywhere, Category = "Player")
	USkeletalMeshComponent* Mesh1P;

public:
	// Sets default values for this character's properties
	AMasterCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Flag")
	FName FlagAttachSocketName;

	UPROPERTY(BlueprintReadOnly)
	int Deaths;



/**
	* Add camera pitch to first person mesh.
	*
	*	@param	CameraLocation	Location of the Camera.
	*	@param	CameraRotation	Rotation of the Camera.
	*/
	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);

	/** get targeting state */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsTargeting() const;


	ASOFPlayerController* PC;
	
	USkeletalMeshComponent* GetPawnMesh() const;

	USkeletalMeshComponent* GetSpecificPawnMesh(bool WantFirstPerson) const;

	static FOnMasterCharacterEquipWeapon NotifyEquipWeapon;
	static FOnMasterCharacteUnEquipWeapon NotifyUnEquipWeapon;

	FName GetWeaponAttachPoint() const;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health Component")
	int32 TeamNum = -1;

	UFUNCTION(BlueprintCallable, Category = Mesh)
	virtual bool IsFirstPerson() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AHitScanWeapon* GetWeapon() const;

	/** check if pawn can fire weapon */
	bool CanFire() const;

	/** check if pawn can reload weapon */
	bool CanReload() const;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;


	/** current firing state */
	uint8 bWantsToFire : 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UHealthComponent* HealthComp;

	void MoveForward(float value);

	void MoveRight(float value);

	void BeginCrouch();

	void EndCrouch();

	/** player pressed start fire action */
	void OnStartFire();

	/** player released start fire action */
	void OnStopFire();

	/** player pressed next weapon action */
	void OnNextWeapon();

	/** player pressed prev weapon action */
	void OnPrevWeapon();

	/** player pressed reload action */
	void OnReload();



	UFUNCTION()
	void OnHealthChanged(UHealthComponent* InHealthComp, float Health, float Armor, float HealthChanged, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCause);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bPlayerDied;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;



	protected:
		/** Returns Mesh1P subobject **/
		FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

};
