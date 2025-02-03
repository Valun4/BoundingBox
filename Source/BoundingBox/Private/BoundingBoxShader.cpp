// Fill out your copyright notice in the Description page of Project Settings.


#include "BoundingBoxShader.h"

IMPLEMENT_SHADER_TYPE(, FBoundingBoxPS, TEXT("/CustomShaders/BoundingBoxShader.usf"), TEXT("BoundingBoxPS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FBoundingBoxCS, TEXT("/CustomShaders/BoundingBoxShader.usf"), TEXT("BoundingBoxCS"), SF_Compute);
