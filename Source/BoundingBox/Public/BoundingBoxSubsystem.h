// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoundingBoxSceneViewExtension.h"
#include "Subsystems/WorldSubsystem.h"
#include "BoundingBoxSubsystem.generated.h"

USTRUCT()
struct FBoundingBoxActor
{
	GENERATED_BODY()

public:
	AActor* actor_;
	FLinearColor bbColor_ = FLinearColor::Green;

	FBoundingBoxActor() {};
    FBoundingBoxActor(AActor* _actor, FLinearColor _bbColor, int _stencilId) : actor_(_actor), bbColor_(_bbColor)
    {
        if (actor_)
        {
			SetCustomStencil(_stencilId);
        }
    }

	const bool IsValid() { return actor_ != nullptr;; }

	void Clear()
	{
		actor_ = nullptr;
	}

	void SetCustomStencil(int _stencilId)
	{
		actor_->ForEachComponent<UMeshComponent>(true, [&](UMeshComponent* component)
		{
			component->bRenderCustomDepth = true;
			component->CustomDepthStencilWriteMask = ERendererStencilMask::ERSM_Default;
			component->CustomDepthStencilValue = ++_stencilId;
			component->MarkRenderStateDirty();
		});
	}

	inline bool operator!=(const FBoundingBoxActor& _other) const { return actor_ != _other.actor_; }
	inline bool operator==(const FBoundingBoxActor& _other) const { return actor_ == _other.actor_; }

};

UCLASS()
class BOUNDINGBOX_API UBoundingBoxSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FBoundingBoxActor> boundingBoxActors_ = {};

	TSharedPtr<FBoundingBoxSceneViewExtension, ESPMode::ThreadSafe> boundingBoxSVE_;

	virtual void Initialize(FSubsystemCollectionBase& _collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float _deltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category = "BoundingBoxSubsystem")
	void AddNewActor(AActor* _actor, FLinearColor _bbColor);

	UFUNCTION(BlueprintCallable, Category = "BoundingBoxSubsystem")
	void StartDrawBoundingBoxes();

	UFUNCTION(BlueprintCallable, Category = "BoundingBoxSubsystem")
	void StopDrawBoundingBoxes();
};
