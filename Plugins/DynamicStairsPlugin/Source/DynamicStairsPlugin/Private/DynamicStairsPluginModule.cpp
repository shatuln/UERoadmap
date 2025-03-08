// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicStairsPluginModule.h"
#include "DynamicStairsPluginEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "DynamicStairsPluginModule"

void FDynamicStairsPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FDynamicStairsPluginEditorModeCommands::Register();
}

void FDynamicStairsPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FDynamicStairsPluginEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicStairsPluginModule, DynamicStairsPluginEditorMode)