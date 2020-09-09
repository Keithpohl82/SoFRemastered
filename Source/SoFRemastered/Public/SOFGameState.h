// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SOFGameState.generated.h"

/**
 * 
 */
UCLASS()
class SOFREMASTERED_API ASOFGameState : public AGameStateBase
{
	GENERATED_BODY()

		ASOFGameState();



public:

virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RedPoints;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 BluePoints;

	FTimerHandle TimerHandle_GameTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
		float GameSeconds;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
		float GameMinutes;

	void SetTimerSeconds();

	void SetTimerMinuets();


	void SetGameTime();

	float GetGameSeconds();

	float GetGameMinuets();

};
