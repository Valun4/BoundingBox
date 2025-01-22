// Fill out your copyright notice in the Description page of Project Settings.

#include "BoundingBoxSceneViewExtension.h"

#include "BoundingBoxShader.h"

#include "ScreenPass.h"
#include "PixelShaderUtils.h"
#include "FXRenderingUtils.h"
#include "DynamicResolutionState.h"
#include "RenderGraphUtils.h"
#include "CommonRenderResources.h"
#include "SceneView.h"

#include "PostProcess/PostProcessMaterialInputs.h"
#include "PostProcess/PostProcessInputs.h"

DEFINE_GPU_STAT(BoundingBoxPass)
FBoundingBoxSceneViewExtension::FBoundingBoxSceneViewExtension(const FAutoRegister& _autoRegister) : FSceneViewExtensionBase(_autoRegister){}

//copy from ColorCorrectRegionsSceneViewExtension.cpp
FScreenPassTextureViewportParameters GetTextureViewportParameters(const FScreenPassTextureViewport& InViewport)
{
    const FVector2f Extent(InViewport.Extent);
    const FVector2f ViewportMin(InViewport.Rect.Min.X, InViewport.Rect.Min.Y);
    const FVector2f ViewportMax(InViewport.Rect.Max.X, InViewport.Rect.Max.Y);
    const FVector2f ViewportSize = ViewportMax - ViewportMin;

    FScreenPassTextureViewportParameters Parameters;

    if (!InViewport.IsEmpty())
    {
        Parameters.Extent = FVector2f(Extent);
        Parameters.ExtentInverse = FVector2f(1.0f / Extent.X, 1.0f / Extent.Y);

        Parameters.ScreenPosToViewportScale = FVector2f(0.5f, -0.5f) * ViewportSize;
        Parameters.ScreenPosToViewportBias = (0.5f * ViewportSize) + ViewportMin;

        Parameters.ViewportMin = InViewport.Rect.Min;
        Parameters.ViewportMax = InViewport.Rect.Max;

        Parameters.ViewportSize = ViewportSize;
        Parameters.ViewportSizeInverse = FVector2f(1.0f / Parameters.ViewportSize.X, 1.0f / Parameters.ViewportSize.Y);

        Parameters.UVViewportMin = ViewportMin * Parameters.ExtentInverse;
        Parameters.UVViewportMax = ViewportMax * Parameters.ExtentInverse;

        Parameters.UVViewportSize = Parameters.UVViewportMax - Parameters.UVViewportMin;
        Parameters.UVViewportSizeInverse = FVector2f(1.0f / Parameters.UVViewportSize.X, 1.0f / Parameters.UVViewportSize.Y);

        Parameters.UVViewportBilinearMin = Parameters.UVViewportMin + 0.5f * Parameters.ExtentInverse;
        Parameters.UVViewportBilinearMax = Parameters.UVViewportMax - 0.5f * Parameters.ExtentInverse;
    }

    return Parameters;
}
void FBoundingBoxSceneViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& _graphBuilder, const FSceneView& _view, const FPostProcessingInputs& _inputs)
{
    if(!dataTexture_.IsValid())
    {
        return;
    }
	_inputs.Validate();

    const FSceneViewFamily& ViewFamily = *_view.Family;
    float ScreenPercentage = ViewFamily.SecondaryViewFraction;

    if (ViewFamily.GetScreenPercentageInterface())
    {
        DynamicRenderScaling::TMap<float> UpperBounds = ViewFamily.GetScreenPercentageInterface()->GetResolutionFractionsUpperBound();
        ScreenPercentage *= UpperBounds[GDynamicPrimaryResolutionFraction];
    }

    const FIntRect viewRect = UE::FXRenderingUtils::GetRawViewRectUnsafe(_view);

    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

    TShaderMapRef<FBoundingBoxVS> VertexShader(GlobalShaderMap);
    check(VertexShader.IsValid());

    // Shader Parameter Setup
    FBoundingBoxVSParams* VertexShaderParams = _graphBuilder.AllocParameters<FBoundingBoxVSParams>();

    FPixelShaderUtils::AddFullscreenPass(
        _graphBuilder,
        GlobalShaderMap,
        FRDGEventName(TEXT("BoundingBox")),
        VertexShader,
        VertexShaderParams,
        viewRect,
        nullptr,
        nullptr,
        nullptr,
        0,
        ERDGPassFlags::Raster
    );

    FScreenPassTexture sceneColor((*_inputs.SceneTextures)->SceneColorTexture, viewRect);

    if (!sceneColor.IsValid())
    {
        return;
    }
    const FScreenPassTextureViewport SceneColorTextureViewport(sceneColor);

    TShaderMapRef<FBoundingBoxPS> PixelShader(GlobalShaderMap);
    check(PixelShader.IsValid());

    FScreenPassRenderTarget sceneColorRenderTargetCopy;
    sceneColorRenderTargetCopy.Texture = _graphBuilder.CreateTexture((*_inputs.SceneTextures)->SceneColorTexture->Desc, TEXT("SceneColor"));

    FBoundingBoxPSParams* PixelShaderParams = _graphBuilder.AllocParameters<FBoundingBoxPSParams>();
    PixelShaderParams->viewParams = GetTextureViewportParameters(SceneColorTextureViewport);
    PixelShaderParams->sceneColor = (*_inputs.SceneTextures)->SceneColorTexture;
    PixelShaderParams->dataTexture = dataTexture_.Pin()->GetResource()->GetTexture2DRHI();
	PixelShaderParams->sceneColorSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();

    PixelShaderParams->RenderTargets[0] = FRenderTargetBinding(sceneColor.Texture, ERenderTargetLoadAction::ELoad);

	FPixelShaderUtils::AddFullscreenPass(
		_graphBuilder,
		GlobalShaderMap,
        FRDGEventName(TEXT("BoundingBox")),
		PixelShader,
		PixelShaderParams,
		viewRect,
		nullptr,
		nullptr,
		nullptr,
		0,
		ERDGPassFlags::Raster
	);
}
