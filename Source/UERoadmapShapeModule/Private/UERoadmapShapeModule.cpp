#include "UERoadmapShapeModule.h"
#include "Modules/ModuleManager.h"

#include "ToolMenus.h"
#include "Components/DynamicMeshComponent.h"

#include "Generators/MinimalBoxMeshGenerator.h"
#include "Generators/SphereGenerator.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "OrientedBoxTypes.h"

IMPLEMENT_GAME_MODULE(FUERoadmapShapeModule, UERoadmapShapeModule);
#define LOCTEXT_NAMESPACE "FMyShapeEditorModule"

void FUERoadmapShapeModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUERoadmapShapeModule::RegisterMenus));
}

void FUERoadmapShapeModule::ShutdownModule()
{
}

void FUERoadmapShapeModule::RegisterMenus()
{
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");

	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("MyTools");

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"CreateBoxButton",
		FUIAction(FExecuteAction::CreateRaw(this, &FUERoadmapShapeModule::OnCreateCube)),
		LOCTEXT("CreateBox_Label", "Box"),
		LOCTEXT("CreateBox_Tooltip", "Create DynamicMesh Box"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.StaticMesh")
	));

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"CreateSphereButton",
		FUIAction(FExecuteAction::CreateRaw(this, &FUERoadmapShapeModule::OnCreateSphere)),
		LOCTEXT("CreateSphere_Label", "Sphere"),
		LOCTEXT("CreateSphere_Tooltip", "Create DynamicMesh Sphere"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.StaticMesh")
	));
}

void FUERoadmapShapeModule::OnCreateCube()
{
	UE::Geometry::FMinimalBoxMeshGenerator MinimalBoxMeshGenerator;
	MinimalBoxMeshGenerator.Box = UE::Geometry::FOrientedBox3d(
            FVector3d(GetPointInFrontOfEditorCamera()),
            FVector3d(50, 50, 50)
        );
    
	MinimalBoxMeshGenerator.Generate();
	FDynamicMesh3 Mesh(&MinimalBoxMeshGenerator);

	UDynamicMeshComponent* MeshComp = GetDynamicMeshComponent();
	if (MeshComp)
	{
		MeshComp->GetMesh()->Copy(Mesh);
    	MeshComp->NotifyMeshUpdated();
	}
}

void FUERoadmapShapeModule::OnCreateSphere()
{
	UE::Geometry::FSphereGenerator SphereGenerator;
	SphereGenerator.Radius = 50;
	SphereGenerator.NumPhi = 16;
	SphereGenerator.NumTheta = 32; 

	SphereGenerator.Generate();
	FDynamicMesh3 Mesh(&SphereGenerator);

	for (int vid : Mesh.VertexIndicesItr())
	{
		Mesh.SetVertex(vid, Mesh.GetVertex(vid) + FVector3d(GetPointInFrontOfEditorCamera()));
	}

	UDynamicMeshComponent* MeshComp = GetDynamicMeshComponent();
	if (MeshComp)
	{
		MeshComp->GetMesh()->Copy(Mesh);
		MeshComp->NotifyMeshUpdated();
	}
}

UDynamicMeshComponent* FUERoadmapShapeModule::GetDynamicMeshComponent()
{
	UDynamicMeshComponent* MeshComp = nullptr;
	if(UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		AActor* Actor = World->SpawnActor<AActor>();
		MeshComp = NewObject<UDynamicMeshComponent>(Actor);
		MeshComp->RegisterComponent();
		Actor->SetRootComponent(MeshComp);
	}
	return MeshComp;
}

FVector FUERoadmapShapeModule::GetPointInFrontOfEditorCamera()
{
    if (GEditor == nullptr)
        return FVector::ZeroVector;

    FEditorViewportClient* ViewportClient = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
    if (ViewportClient == nullptr)
        return FVector::ZeroVector;
	
    const FVector CamLoc = ViewportClient->GetViewLocation();
    const FRotator CamRot = ViewportClient->GetViewRotation();
	
    const FVector Forward = CamRot.Vector();
	
    return CamLoc + Forward * 500.0f;
}