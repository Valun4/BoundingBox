// Fill out your copyright notice in the Description page of Project Settings.


#include "BoundingBoxSubsystem.h"

void UBoundingBoxSubsystem::Initialize(FSubsystemCollectionBase& _collection)
{
	boundingBoxSVE_ = FSceneViewExtensions::NewExtension<FBoundingBoxSceneViewExtension>();

	if (!dataTexture_)
	{
		TArray<float>arrData;
		arrData.Init(0.0f, maxSize_ * vertexDataSize_);

		dataTexture_ = UTexture2D::CreateTransient(maxSize_ * vertexDataSize_, 1, PF_R32_FLOAT);

		dataTexture_->NeverStream = true;
		dataTexture_->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

		FTexture2DMipMap& Mip = dataTexture_->GetPlatformData()->Mips[0];

		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

		FMemory::Memcpy(Data, arrData.GetData(), arrData.Num() * 4);
		Mip.BulkData.Unlock();

		dataTexture_->UpdateResource();
	}
	boundingBoxSVE_->dataTexture_ = dataTexture_;
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

	dataTexture_ = nullptr;

	Super::Deinitialize();
}

void UBoundingBoxSubsystem::Tick(float _deltaTime)
{
	for(auto& bbActor : boundingBoxActors_)
	{
		bbActor.UpdateBBActor();
	}
	UpdateDataTexture();
	Super::Tick(_deltaTime);
}

TStatId UBoundingBoxSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBoundingBoxSubsystem, STATGROUP_Tickables);
}

void UBoundingBoxSubsystem::AddNewActor(AActor* _actor, FLinearColor _bbColor)
{
	boundingBoxActors_.AddUnique({ _actor, _bbColor });
}

void UBoundingBoxSubsystem::UpdateDataTexture()
{
	//buffer texture
	//0 -> count of bboxes
	//1-8 for screenspace 4 positions
	//9-12 for color

	if(dataTexture_)
	{
		TArray<float>arrData;
		arrData.Init(-1.0f, maxSize_ * vertexDataSize_);
		arrData[0] = boundingBoxActors_.Num();
		int beginOffset = 1;
		for(int i= 0; i < boundingBoxActors_.Num(); i++)
		{
			//write screen space positions
			arrData[beginOffset + 0 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[0].X;
			arrData[beginOffset + 1 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[0].Y;
			arrData[beginOffset + 2 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[1].X;
			arrData[beginOffset + 3 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[1].Y;
			arrData[beginOffset + 4 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[2].X;
			arrData[beginOffset + 5 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[2].Y;
			arrData[beginOffset + 6 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[3].X;
			arrData[beginOffset + 7 + i * vertexDataSize_] = boundingBoxActors_[i].sSvertices_[3].Y;

			//write color
			arrData[beginOffset + 8 + i * vertexDataSize_] = boundingBoxActors_[i].bbColor_.R;
			arrData[beginOffset + 9 + i * vertexDataSize_] = boundingBoxActors_[i].bbColor_.G;
			arrData[beginOffset + 10 + i * vertexDataSize_] = boundingBoxActors_[i].bbColor_.B;
		}

		FTexture2DMipMap& Mip = dataTexture_->GetPlatformData()->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, arrData.GetData(), arrData.Num() * 4);
		Mip.BulkData.Unlock();
		dataTexture_->UpdateResource();
	}
}

