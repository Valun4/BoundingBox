// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"
#include "ScreenPass.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 16

#pragma region ComputeShader
BEGIN_SHADER_PARAMETER_STRUCT(FBoundingBoxCSParams, )
	SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<uint2>, customStencil)
	SHADER_PARAMETER_RDG_BUFFER_UAV(StructuredBuffer<float4>, boundingBoxDataBuffer)
	SHADER_PARAMETER(int, boundingBoxCount)
END_SHADER_PARAMETER_STRUCT()

class FBoundingBoxCS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_SHADER_TYPE(FBoundingBoxCS, Global,);
	using FParameters = FBoundingBoxCSParams;
	SHADER_USE_PARAMETER_STRUCT(FBoundingBoxCS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5); }

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads); //to to enable uavs in shader

		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}
};

#pragma endregion
#pragma region PixelShader
BEGIN_SHADER_PARAMETER_STRUCT(FBoundingBoxPSParams, )
	SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, viewParams)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, sceneColor)
	SHADER_PARAMETER_SAMPLER(SamplerState, sceneColorSampler)
	SHADER_PARAMETER_RDG_BUFFER_UAV(StructuredBuffer<float4>, boundingBoxDataBuffer)
	SHADER_PARAMETER(int, boundingBoxCount)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()
class FBoundingBoxPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoundingBoxPS);
	using FParameters = FBoundingBoxPSParams;
	SHADER_USE_PARAMETER_STRUCT(FBoundingBoxPS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5); }

};

#pragma endregion