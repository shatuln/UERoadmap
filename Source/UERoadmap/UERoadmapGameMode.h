// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UERoadmapSaveGame.h"
#include "UERoadmapGameMode.generated.h"

UCLASS(minimalapi)
class AUERoadmapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUERoadmapGameMode();

	virtual void BeginPlay() override;

	void SaveGameInGameMode(UUERoadmapSaveGame* SaveGameInstance);
	void LoadGameInGameMode(UUERoadmapSaveGame* LoadGameInstance);
};



