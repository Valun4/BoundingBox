// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SceneViewExtension.h"
#include "BoundingBoxShader.h"

DECLARE_GPU_STAT_NAMED_EXTERN(BoundingBoxPass, TEXT("BoundingBoxPass"))
class BOUNDINGBOX_API FBoundingBoxSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FBoundingBoxSceneViewExtension(const FAutoRegister& _autoRegister);
	virtual ~FBoundingBoxSceneViewExtension() override;

	virtual void SetupViewFamily(FSceneViewFamily& _viewFamily) override {};
	virtual void SetupView(FSceneViewFamily& _viewFamily, FSceneView& _view) override {};
	virtual void BeginRenderViewFamily(FSceneViewFamily& _viewFamily) override {};
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& _graphBuilder, const FSceneView& _view, const FPostProcessingInputs& _inputs) override;

	bool bDrawAllowed = false;
	int boundingBoxCount = 0;
	TRefCountPtr<IPooledRenderTarget> computeShaderOutput;

	//FRHIGPUBufferReadback* readback = nullptr; //readback pointer
	//TArray<FVector4f> boundingBoxes = {};		 //bounding boxdata

};
