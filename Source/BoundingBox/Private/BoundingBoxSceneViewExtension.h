// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SceneViewExtension.h"
#include "RHI.h"
#include "RHIResources.h"

DECLARE_GPU_STAT_NAMED_EXTERN(BoundingBoxPass, TEXT("BoundingBoxPass")) //declares name of the render pass for render debugger
class BOUNDINGBOX_API FBoundingBoxSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FBoundingBoxSceneViewExtension(const FAutoRegister& _autoRegister);

	virtual void SetupViewFamily(FSceneViewFamily& _viewFamily) override {};
	virtual void SetupView(FSceneViewFamily& _viewFamily, FSceneView& _view) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& _viewFamily) override {};
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& _graphBuilder, const FSceneView& _view, const FPostProcessingInputs& _inputs) override;

	UPROPERTY()
	TWeakObjectPtr<UTexture2D> dataTexture_ = nullptr;
};
