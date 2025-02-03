// Fill out your copyright notice in the Description page of Project Settings.

#include "BoundingBoxSceneViewExtension.h"

#include "PixelShaderUtils.h"
#include "FXRenderingUtils.h"
#include "PostProcess/PostProcessInputs.h"


DEFINE_GPU_STAT(BoundingBoxPass)
FBoundingBoxSceneViewExtension::FBoundingBoxSceneViewExtension(const FAutoRegister& _autoRegister) : FSceneViewExtensionBase(_autoRegister)
{
    //readback = new FRHIGPUBufferReadback("Bounding Box Info Readback");
}

FBoundingBoxSceneViewExtension::~FBoundingBoxSceneViewExtension()
{
    //delete readback;
    //readback = nullptr;
}


//copy paste from ColorCorrectRegionsSceneViewExtension.cpp
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
    if (!bDrawAllowed) { return; }

    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

    const FSceneViewFamily& ViewFamily = *_view.Family;
    const FIntRect viewRect = UE::FXRenderingUtils::GetRawViewRectUnsafe(_view);

#pragma region Compute Shader

    if (boundingBoxCount == 0)
    {
        return;
    }

    //compute shader buffers
    FRDGBufferRef boundingBoxDataBuffer = nullptr;
    FRDGBufferUAVRef boundingBoxDataBufferUAV = nullptr;

    const FRDGBufferDesc boundingBoxDataDesc = FRDGBufferDesc::CreateStructuredDesc(sizeof(FVector4), boundingBoxCount);
    boundingBoxDataBuffer = _graphBuilder.CreateBuffer(boundingBoxDataDesc, TEXT("Bounding Box Count Buffer"));
    boundingBoxDataBufferUAV = _graphBuilder.CreateUAV(boundingBoxDataBuffer);

    //create CS params
    FBoundingBoxCS::FParameters* computeShaderParams = _graphBuilder.AllocParameters<FBoundingBoxCS::FParameters>();
    computeShaderParams->customStencil = (*_inputs.SceneTextures)->CustomStencilTexture;
    computeShaderParams->boundingBoxCount = boundingBoxCount;
    computeShaderParams->boundingBoxDataBuffer = boundingBoxDataBufferUAV;
    
    const FIntPoint threadCount = 1;
    const FIntVector groupCount = FComputeShaderUtils::GetGroupCount(1, FIntPoint(1, 1));

    FComputeShaderUtils::AddPass(_graphBuilder,
        RDG_EVENT_NAME("Get bounding box data from stencil buffer"),
        ERDGPassFlags::Compute,
        TShaderMapRef<FBoundingBoxCS>(GlobalShaderMap),
        computeShaderParams,
        groupCount);

#pragma endregion
#pragma region Pixel Shader

    FScreenPassTexture sceneColor((*_inputs.SceneTextures)->SceneColorTexture, viewRect);

    const FScreenPassTextureViewport SceneColorTextureViewport(sceneColor);

    FBoundingBoxPSParams* PixelShaderParams = _graphBuilder.AllocParameters<FBoundingBoxPSParams>();
    PixelShaderParams->viewParams = GetTextureViewportParameters(SceneColorTextureViewport);
    PixelShaderParams->sceneColor = (*_inputs.SceneTextures)->SceneColorTexture;
    PixelShaderParams->sceneColorSampler = TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
    PixelShaderParams->boundingBoxDataBuffer = boundingBoxDataBufferUAV;
    PixelShaderParams->boundingBoxCount = boundingBoxCount;
    PixelShaderParams->RenderTargets[0] = FRenderTargetBinding(sceneColor.Texture, ERenderTargetLoadAction::ELoad);
   
    FPixelShaderUtils::AddFullscreenPass(
        _graphBuilder,
        GlobalShaderMap,
        FRDGEventName(TEXT("Draw bounding box post process")),
        TShaderMapRef<FBoundingBoxPS>(GlobalShaderMap),
        PixelShaderParams,
        viewRect,
        nullptr,
        nullptr,
        nullptr,
        0,
        ERDGPassFlags::Raster
    );
#pragma endregion
}