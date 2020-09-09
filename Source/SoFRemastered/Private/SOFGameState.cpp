// Fill out your copyright notice in the Description page of Project Settings.


#include "SOFGameState.h"
#include "Net/UnrealNetwork.h"


ASOFGameState::ASOFGameState()
{
	
}





void ASOFGameState::BeginPlay()
{
	Super::BeginPlay();
	GameSeconds = 0;
	GameMinutes = 20;
	SetGameTime();
}

void ASOFGameState::SetTimerSeconds()
{
	--GameSeconds;

	if (GameSeconds <= 0)
	{
		SetTimerMinuets();
		GameSeconds = 59;
	}
}

void ASOFGameState::SetTimerMinuets()
{
	--GameMinutes;
}

void ASOFGameState::SetGameTime()
{
	GetWorldTimerManager().SetTimer(TimerHandle_GameTimer, this, &ASOFGameState::SetTimerSeconds, 1.0f, true, 1.0f);
}

float ASOFGameState::GetGameSeconds()
{
	return GameSeconds;
}

float ASOFGameState::GetGameMinuets()
{
	return GameMinutes;
}

void ASOFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASOFGameState, RedPoints);

	DOREPLIFETIME(ASOFGameState, BluePoints);
}