#include "DynamicStairsPluginMyStairsTool.h"

UInteractiveTool* UDynamicStairsPluginMyStairsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UDynamicStairsPluginMyStairsTool* NewTool = NewObject<UDynamicStairsPluginMyStairsTool>();
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

UDynamicStairsPluginMyStairsToolProperties::UDynamicStairsPluginMyStairsToolProperties()
{
	PointToSpawn = FVector::ZeroVector;
}

void UDynamicStairsPluginMyStairsToolProperties::OnButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("My Stairs Tool Button Clicked!"));
}

void UDynamicStairsPluginMyStairsTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void UDynamicStairsPluginMyStairsTool::Setup()
{
	UInteractiveTool::Setup();

	// Create the property set and register it with the Tool
	Properties = NewObject<UDynamicStairsPluginMyStairsToolProperties>(this, "Spawning");
	AddToolPropertySource(Properties);
	
	UClass* LoadedBP = LoadObject<UClass>(nullptr, TEXT("/DynamicStairsPlugin/Blueprints/BP_Stairs.BP_Stairs_C"));
	if (LoadedBP)
	{
		MyBlueprintClass = LoadedBP;
	}
}

void UDynamicStairsPluginMyStairsTool::SpawnStairs()
{
}
