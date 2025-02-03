// Fill out your copyright notice in the Description page of Project Settings.


#include "BoundingBoxSubsystem.h"

void UBoundingBoxSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	boundingBoxSVE_ = FSceneViewExtensions::NewExtension<FBoundingBoxSceneViewExtension>();
	Super::Initialize(_collection);
}

void UBoundingBoxSubsystem::Deinitialize()
{
	for(auto& bbActor : boundingBoxActors_) { bbActor.Clear();}
	boundingBoxActors_ = {};

	if(boundingBoxSVE_)
	{
		boundingBoxSVE_.Reset();
		boundingBoxSVE_ = nullptr;
	}

	Super::Deinitialize();
}

void UBoundingBoxSubsystem::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
}

TStatId UBoundingBoxSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBoundingBoxSubsystem, STATGROUP_Tickables);
}

void UBoundingBoxSubsystem::AddNewActor(AActor* _actor, FLinearColor _bbColor)
{
	boundingBoxActors_.Add({ _actor, _bbColor, boundingBoxActors_.Num() });
	boundingBoxSVE_->boundingBoxCount++;		
	
}

void UBoundingBoxSubsystem::StartDrawBoundingBoxes()
{
	boundingBoxSVE_->bDrawAllowed = true;
}

void UBoundingBoxSubsystem::StopDrawBoundingBoxes()
{
	boundingBoxSVE_->bDrawAllowed = false;
}
