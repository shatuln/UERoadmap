#pragma once
#include "Components/DynamicMeshComponent.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FUERoadmapShapeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void OnCreateCube();
	void OnCreateSphere();
	UDynamicMeshComponent* GetDynamicMeshComponent();
	FVector GetPointInFrontOfEditorCamera();
};