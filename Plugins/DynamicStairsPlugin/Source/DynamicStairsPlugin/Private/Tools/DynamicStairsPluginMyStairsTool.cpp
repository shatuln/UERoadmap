#include "DynamicStairsPluginMyStairsTool.h"

#include "BaseBehaviors/SingleClickBehavior.h"

UInteractiveTool* UDynamicStairsPluginMyStairsToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UDynamicStairsPluginMyStairsTool* NewTool = NewObject<UDynamicStairsPluginMyStairsTool>();
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

UDynamicStairsPluginMyStairsToolProperties::UDynamicStairsPluginMyStairsToolProperties()
{
	PointToSpawn = FVector::ZeroVector;
	NumSteps = 10;
	StaircaseTop = FVector(240.0f, 50.0f, 240.0f);
}

void UDynamicStairsPluginMyStairsToolProperties::SpawnStairs()
{
	UE_LOG(LogTemp, Warning, TEXT("My Stairs Tool Button Clicked!"));
	if (MyStairsTool)
	{
		MyStairsTool->SpawnStairs();
	}
}

void UDynamicStairsPluginMyStairsTool::SetWorld(UWorld* World)
{
	check(World);
	this->TargetWorld = World;
}

void UDynamicStairsPluginMyStairsTool::Setup()
{
	UInteractiveTool::Setup();

	USingleClickInputBehavior* MouseBehavior = NewObject<USingleClickInputBehavior>();
	MouseBehavior->Initialize(this);
	AddInputBehavior(MouseBehavior);
	
	// Create the property set and register it with the Tool
	Properties = NewObject<UDynamicStairsPluginMyStairsToolProperties>(this, "Spawning");
	Properties->MyStairsTool = this;
	AddToolPropertySource(Properties);
	
	UClass* LoadedBP = LoadObject<UClass>(nullptr, TEXT("/DynamicStairsPlugin/Blueprints/BP_Stairs.BP_Stairs_C"));
	if (LoadedBP)
	{
		MyBlueprintClass = LoadedBP;
	}
}

void UDynamicStairsPluginMyStairsTool::Render(IToolsContextRenderAPI* RenderAPI)
{
	FPrimitiveDrawInterface* PDI = RenderAPI->GetPrimitiveDrawInterface();
	if (PDI)
	{
		PDI->DrawPoint(Properties->PointToSpawn, FColor(240, 16, 16), 50.0f, SDPG_Foreground);
	}
}

void UDynamicStairsPluginMyStairsTool::OnClicked(const FInputDeviceRay& PressPos)
{
	
}

FInputRayHit UDynamicStairsPluginMyStairsTool::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllObjects);
	FHitResult Result;
	bool bHitWorld = TargetWorld->LineTraceSingleByObjectType(Result, ClickPos.WorldRay.Origin, ClickPos.WorldRay.PointAt(999999), QueryParams);
	if (bHitWorld)
	{
		Properties->PointToSpawn = Result.ImpactPoint;
		return FInputRayHit(Result.Distance);
	}
	return FInputRayHit();
}

void UDynamicStairsPluginMyStairsTool::SpawnStairs()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	AActor* SpawnedActor = TargetWorld->SpawnActor<AActor>(MyBlueprintClass, Properties->PointToSpawn, SpawnRotation, SpawnParams);
	if (SpawnedActor)
	{
		FProperty* NumStepsProperty = SpawnedActor->GetClass()->FindPropertyByName(TEXT("Num Steps"));
		if (NumStepsProperty)
		{
			*NumStepsProperty->ContainerPtrToValuePtr<int>(SpawnedActor) = Properties->NumSteps;
		}
		
		FProperty* StaircaseTopProperty = SpawnedActor->GetClass()->FindPropertyByName(TEXT("Staircase Top"));
		if (StaircaseTopProperty)
		{
			*StaircaseTopProperty->ContainerPtrToValuePtr<FVector>(SpawnedActor) = Properties->StaircaseTop;
		}
		
		FProperty* StaircaseDirectionProperty = SpawnedActor->GetClass()->FindPropertyByName(TEXT("More Stairs"));
		if (StaircaseDirectionProperty)
		{
			*StaircaseDirectionProperty->ContainerPtrToValuePtr<TArray<EStaircaseDirection>>(SpawnedActor) = Properties->StaircaseDirection;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Stairs spawned successfully with name %s!"), *SpawnedActor->GetName());
	}
}
