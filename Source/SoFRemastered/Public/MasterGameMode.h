// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "../SoFRemastered.h"
#include "GameFramework/GameModeBase.h"
#include "SOFPlayerController.h"
#include "SOFGameInstance.h"


#include "MasterGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController, AController*, VictimController);


class ASPlayerState;

UCLASS()
class SOFREMASTERED_API AMasterGameMode : public AGameModeBase
{
	GENERATED_BODY()

	

public:

	virtual void Tick(float DeltaTime) override;

	virtual void StartPlay() override;

	void FlagCapture(uint8 TeamThatCapturedIt);

		/** can players damage each other? */
		virtual bool CanDealDamage(ASPlayerState* DamageInstigator, ASPlayerState* DamagedPlayer) const;

	FTimerHandle TimerHandle_RespawnTimer;



	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnTime;



	int32 RedPlayerCount;

	int32 BluePlayerCount;

	UPROPERTY(BlueprintAssignable)
	FOnActorKilled OnActorKilled;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* CaptureSound;



	void PlayerRespawn();

	void RestartDeadPlayer();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;


};
