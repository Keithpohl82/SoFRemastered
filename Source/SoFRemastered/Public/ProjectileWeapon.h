// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SOFREMASTERED_API AProjectileWeapon : public AHitScanWeapon
{
	GENERATED_BODY()

	virtual void Fire() override;


public:
	// Need reload function 

	UPROPERTY(EditDefaultsOnly, Category = projectile)
	TSubclassOf<AActor> ProjectileClass;

};
