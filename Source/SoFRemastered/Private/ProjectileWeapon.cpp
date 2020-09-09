// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"



void AProjectileWeapon::Fire()
{
	if (Ammo > 0)
	{
		Ammo--;

		AActor* MyOwner = GetOwner();

		if (MyOwner && ProjectileClass)
		{


			FVector EyeLocation;
			FRotator EyeRotation;


			APawn* PawnOwner = Cast<APawn>(GetOwner());
			AController* ControllerOwner = PawnOwner->GetController();
			ControllerOwner->GetPlayerViewPoint(EyeLocation, EyeRotation);

			FVector MuzzleLocation = GunMesh1P->GetSocketLocation(MuzzleSocketName);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (bIsShotGun)
			{
				for (int32 i = 0; i < 8; i++)
				{
					GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
				}
			}
			else
			{
				GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
			}
			//GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
		}
	}

}
