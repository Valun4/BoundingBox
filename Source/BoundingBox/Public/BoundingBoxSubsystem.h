// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoundingBoxSceneViewExtension.h"
#include "Subsystems/WorldSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "BoundingBoxSubsystem.generated.h"

USTRUCT()
struct FBoundingBoxActor
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<AActor> actor_;
	FVector origin_ = FVector::Zero();
	FVector extent_ = FVector::Zero();
	FRotator initialRot = FRotator::ZeroRotator;
	TArray<FVector> oBBvertices_ = {};
	TArray<FVector2D> sSvertices_ = {};
	FLinearColor bbColor_ = FLinearColor::Green;

	FBoundingBoxActor() {};
    FBoundingBoxActor(AActor* _actor, FLinearColor _bbColor) : actor_(_actor), bbColor_(_bbColor)
    {
        if (actor_.IsValid())
        {
            actor_.Pin()->GetActorBounds(false, origin_, extent_, true);
			initialRot = actor_->GetActorRotation();
        }
    }

	const bool IsValid() { return actor_.IsValid(); }

	void UpdateBBActor()
	{
		if(IsValid())
		{
			UpdateObbVertices();
			UpdateSsVertices();
		}
	}

	void Clear()
	{
		actor_ = nullptr;
		origin_ = FVector::Zero();
		extent_ = FVector::Zero();
		oBBvertices_.Empty();
		sSvertices_.Empty();
	}

	inline bool operator!=(const FBoundingBoxActor& _other) const { return actor_ != _other.actor_; }
	inline bool operator==(const FBoundingBoxActor& _other) const { return actor_ == _other.actor_; }

private:
	void UpdateObbVertices()
	{
		if(IsValid())
		{
			oBBvertices_.Empty();
			
			FVector right = actor_.Pin()->GetActorRightVector();
			FVector up = actor_.Pin()->GetActorUpVector();
			FVector forward = actor_.Pin()->GetActorForwardVector();
			
			oBBvertices_.Add(FVector(origin_ + right * extent_.X + forward * extent_.Y + up * extent_.Z));
			oBBvertices_.Add(FVector(origin_ + -right * extent_.X + forward * extent_.Y + up * extent_.Z));
			oBBvertices_.Add(FVector(origin_ + -right * extent_.X + -forward * extent_.Y + up * extent_.Z));
			oBBvertices_.Add(FVector(origin_ + right * extent_.X + -forward * extent_.Y + up * extent_.Z));

			oBBvertices_.Add(FVector(origin_ + right * extent_.X + forward * extent_.Y + -up * extent_.Z));
			oBBvertices_.Add(FVector(origin_ + -right * extent_.X + forward * extent_.Y + -up * extent_.Z));
			oBBvertices_.Add(FVector(origin_ + -right * extent_.X + -forward * extent_.Y + -up * extent_.Z));
			oBBvertices_.Add(FVector(origin_ + right * extent_.X + -forward * extent_.Y + -up * extent_.Z));
		}
	}

	void UpdateSsVertices()
	{
		APlayerController* playerController = UGameplayStatics::GetPlayerController(actor_.Pin()->GetWorld() ,0);
		sSvertices_.Init(FVector2d::Zero(), 4);
		TArray<FVector2D> tempVertices = {};

		for(auto& vertex : oBBvertices_)
		{
			FVector2D screenPos;
			playerController->ProjectWorldLocationToScreen(vertex, screenPos, true);
			tempVertices.Add(screenPos);
		}

		// winding order
		//	0 1
		//	3 2		
		tempVertices.Sort([](const FVector2D _a, const FVector2D _b) { return _a.X > _b.X; });
		sSvertices_[1].X = tempVertices[0].X;
		sSvertices_[2].X = tempVertices[0].X;

		tempVertices.Sort([](const FVector2D _a, const FVector2D _b) { return _a.X < _b.X; });
		sSvertices_[0].X = tempVertices[0].X;
		sSvertices_[3].X = tempVertices[0].X;

		tempVertices.Sort([](const FVector2D _a, const FVector2D _b) { return _a.Y > _b.Y; });
		sSvertices_[0].Y = tempVertices[0].Y;
		sSvertices_[1].Y = tempVertices[0].Y;

		tempVertices.Sort([](const FVector2D _a, const FVector2D _b) { return _a.Y < _b.Y; });
		sSvertices_[3].Y = tempVertices[0].Y;
		sSvertices_[2].Y = tempVertices[0].Y;
	}
};

UCLASS()
class BOUNDINGBOX_API UBoundingBoxSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FBoundingBoxActor> boundingBoxActors_ = {};

	UPROPERTY()
	TObjectPtr<UTexture2D> dataTexture_ = nullptr;

	TSharedPtr<FBoundingBoxSceneViewExtension, ESPMode::ThreadSafe> boundingBoxSVE_;

	virtual void Initialize(FSubsystemCollectionBase& _collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float _deltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category = "BoundingBoxSubsystem")
	void AddNewActor(AActor* _actor, FLinearColor _bbColor);

	void UpdateDataTexture();
	
private:
	const uint32 maxSize_ = 100;
	const uint32 vertexDataSize_ = 11;

};
