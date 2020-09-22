#pragma once
#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "UObject/ObjectMacros.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimMirroringData.h"

#include "AnimNode_AnimCurveMirroring.generated.h"

// Anim Behavior Node
USTRUCT(BlueprintInternalUseOnly)
struct ANIMMIRRORING_API FAnimNode_AnimCurveMirroring : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	TArray<bool> ProccessedCurve;

	// FAnimNode_Base interface
	// virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	// End of FAnimNode_Base interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FPoseLink Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	TArray<FMirroringAnimCurveDefine> MirroringData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	bool bEnabled;

	FAnimNode_AnimCurveMirroring();
};
