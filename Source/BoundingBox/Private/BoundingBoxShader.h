// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"
#include "ScreenPass.h"


BEGIN_SHADER_PARAMETER_STRUCT(FBoundingBoxVSParams,)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FBoundingBoxVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoundingBoxVS);
	using FParameters = FBoundingBoxVSParams;
	SHADER_USE_PARAMETER_STRUCT(FBoundingBoxVS, FGlobalShader);

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& _parameters) { return true; }

};

//define a shader struct to so we dont have to bind every single parameter separetely
//SHADER_USE_PARAMETER_STRUCT to automatically bind all the struct parameters
BEGIN_SHADER_PARAMETER_STRUCT(FBoundingBoxPSParams, )
	SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, viewParams)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, sceneColor)
	SHADER_PARAMETER_TEXTURE(Texture2D, dataTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, sceneColorSampler) 
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()
class FBoundingBoxPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoundingBoxPS);
	using FParameters = FBoundingBoxPSParams;
	SHADER_USE_PARAMETER_STRUCT(FBoundingBoxPS, FGlobalShader);

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& _parameters) { return true; }
};