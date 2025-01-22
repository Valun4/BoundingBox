// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoundingBox/BoundingBoxModule.h"

void FBoundingBoxModule::StartupModule()
{
	FString shaderDir = FPaths::GameSourceDir() + "BoundingBox/Shaders/";
	AddShaderSourceDirectoryMapping(TEXT("/CustomShaders"), shaderDir);
}

void FBoundingBoxModule::ShutdownModule()
{

}

IMPLEMENT_MODULE( FBoundingBoxModule, BoundingBox );
