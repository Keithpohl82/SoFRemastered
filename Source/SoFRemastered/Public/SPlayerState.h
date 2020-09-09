// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"


UENUM(BlueprintType)
enum class ETeams : uint8
{
	Spectator UMETA(DisplayName = "Spectator"),
	RedTeam UMETA(DisplayName = "Red Team"),
	BlueTeam UMETA(DisplayName = "Blue Team")
};


UCLASS()
class SOFREMASTERED_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ASPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 TeamNum;
		

protected:

	

public:


	UPROPERTY(BlueprintReadWrite, Replicated)
	float PlayerScore;
	float PlayerKills;
	UPROPERTY(BlueprintReadWrite, Replicated)
	float PlayerDeaths;
	float PlayerKDA;

	UFUNCTION(BlueprintCallable)
	void AddScore(float ScoreDelta);

	UFUNCTION(BlueprintPure)
	int GetPlayerScore();

	UFUNCTION(BlueprintCallable)
	void AddDeath(float ScoreDelta);
	
};
