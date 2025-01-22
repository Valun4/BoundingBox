// Fill out your copyright notice in the Description page of Project Settings.


#include "BoundingBoxShader.h"

//Define Shaders
IMPLEMENT_SHADER_TYPE(, FBoundingBoxVS, TEXT("/CustomShaders/BoundingBoxShader.usf"), TEXT("BoundingBoxVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FBoundingBoxPS, TEXT("/CustomShaders/BoundingBoxShader.usf"), TEXT("BoundingBoxPS"), SF_Pixel);
