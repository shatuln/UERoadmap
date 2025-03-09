#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
#include "DynamicStairsPluginMyStairsTool.generated.h"

UENUM()
enum class EStaircaseDirection : uint8
{
	Forward,
	Right,
	Backward,
	Left
};

/**
 * Builder for UDynamicStairsPluginMyStairsToolBuilder
 */
UCLASS()
class DYNAMICSTAIRSPLUGIN_API UDynamicStairsPluginMyStairsToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

/**
 * Property set for the UDynamicStairsPluginMyStairsTool
 */
UCLASS(Transient)
class DYNAMICSTAIRSPLUGIN_API UDynamicStairsPluginMyStairsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UDynamicStairsPluginMyStairsToolProperties();
	
	/** First point of measurement */
	UPROPERTY(EditAnywhere, Category = Options)
	FVector PointToSpawn;

	UPROPERTY(EditAnywhere, Category = Options, meta=(ClampMin=2))
	int NumSteps;
	
	UPROPERTY(EditAnywhere, Category = Options)
	FVector StaircaseTop;

	UPROPERTY()
	TObjectPtr<UDynamicStairsPluginMyStairsTool> MyStairsTool;

	UPROPERTY(EditAnywhere, Category = Options)
	TArray<EStaircaseDirection> StaircaseDirection;
	
	UFUNCTION(CallInEditor)
	void SpawnStairs();
};

UCLASS()
class DYNAMICSTAIRSPLUGIN_API UDynamicStairsPluginMyStairsTool : public UInteractiveTool, public IClickBehaviorTarget//, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	virtual void SetWorld(UWorld* World);

	/** UInteractiveTool overrides */
	virtual void Setup() override;
	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

	/** IClickDragBehaviorTarget implementation */
	virtual void OnClicked(const FInputDeviceRay& PressPos) override;
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;

	void SpawnStairs();


protected:
	/** Properties of the tool are stored here */
	UPROPERTY()
	TObjectPtr<UDynamicStairsPluginMyStairsToolProperties> Properties;

	UWorld* TargetWorld = nullptr;		// target World we will raycast into

private:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> MyBlueprintClass;
};