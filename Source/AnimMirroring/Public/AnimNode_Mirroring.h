#pragma once
#include "CoreMinimal.h"
#include "Containers/Map.h"
#include "UObject/ObjectMacros.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimMirroringData.h"

#include "AnimNode_Mirroring.generated.h"

struct FBoneContainer;

/** ミラーリング対象 */
USTRUCT()
struct FMirroringTarget
{
	GENERATED_BODY()

	UPROPERTY()
	FBoneReference BoneRef;

	UPROPERTY()
	FBoneReference CounterpartBoneRef;

	UPROPERTY()
	EMirroringAxis MirrorAxis;
};


// Anim Behavior Node
USTRUCT(BlueprintInternalUseOnly)
struct ANIMMIRRORING_API FAnimNode_Mirroring : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	UAnimMirroringData* MirroringData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	TArray<FMirroringTargetDefine> OverrideMirroringData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault))
	bool MirroringEnable;

	void Reset();

protected:
	TArray<FMirroringTarget> Targets;
	void UpdateTargets(const FBoneContainer& RequiredBones);

	TArray<FTransform> CSRefPoseTransforms;
	void UpdateCSRefPoseTransforms(const FBoneContainer& RequiredBones);

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

public:
	// FAnimNode_Base interface
	//virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	//virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	// End of FAnimNode_Base interface

	FAnimNode_Mirroring();
};
