#include "AnimNode_Mirroring.h"
#include "AnimMirroringLog.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimationAsset.h"


namespace {
	inline FQuat HadamardProduct(const FQuat& q, const FVector4& v)
	{
		return FQuat(q.X * v.X, q.Y * v.Y, q.Z * v.Z, q.W * v.W);
	};

	inline void MirrorTransform(FTransform& InOutTransform, EMirrorAxis MirrorAxis)
	{
		switch(MirrorAxis) {
		case EMirrorAxis::XAxis:
			InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(-1.0f, 1.0f, 1.0f));
			InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(1.0f, -1.0f, -1.0f, 1.0f)));
			break;
		case EMirrorAxis::YAxis:
			InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(1.0f, -1.0f, 1.0f));
			InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(-1.0f, 1.0f, -1.0f, 1.0f)));
			break;
		case EMirrorAxis::ZAxis:
			InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(1.0f, 1.0f, -1.0f));
			InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(-1.0f, -1.0f, 1.0f, 1.0f)));
			break;
		}
	}
}


FAnimNode_Mirroring::FAnimNode_Mirroring()
	: FAnimNode_SkeletalControlBase()
	, MirroringData(nullptr)
	, MirroringEnable(true)
{
}


void FAnimNode_Mirroring::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);

	const FBoneContainer& boneContainer = Context.AnimInstanceProxy->GetRequiredBones();
	
	Targets.Reserve(boneContainer.GetNumBones());
	Targets.Empty();

	for (int i = 0; i < boneContainer.GetCompactPoseNumBones(); i++)
	{
		auto skeletonIndex = boneContainer.GetSkeletonIndex(FCompactPoseBoneIndex(i));

		if (skeletonIndex >= boneContainer.GetNumBones())
		{
			continue;
		}

		auto boneName = boneContainer.GetReferenceSkeleton().GetBoneName(skeletonIndex);

		for (auto j : Targets)
		{
			if(j.BoneRef.BoneName == boneName || (j.CounterpartBoneRef.BoneName.IsNone() && j.CounterpartBoneRef.BoneName == boneName)) continue;
		}

		FString counterBoneNameString;
		auto mirrorAxis = FMirrorMatchData::FindMirrorAxis(OverrideMirrorMatches, boneName.ToString(), counterBoneNameString);

		if(mirrorAxis == EMirrorAxis::None) mirrorAxis = MirroringData->FindMirrorAxis(boneName.ToString(), counterBoneNameString);

		if (mirrorAxis != EMirrorAxis::None)
		{
			FMirrorTarget target;
			target.MirrorAxis = mirrorAxis;

			target.BoneRef.BoneName = boneName;

			if (!counterBoneNameString.IsEmpty())
			{
				FName counterBoneName(*counterBoneNameString);
				if (boneContainer.GetPoseBoneIndexForBoneName(counterBoneName) >= 0)
				{
					target.CounterpartBoneRef.BoneName = counterBoneName;
				}
			}
			Targets.Add(target);
		}
	}
}


void FAnimNode_Mirroring::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	if (!MirroringEnable)
	{
		return;
	}

	FTransform componentTransform = Output.AnimInstanceProxy->GetComponentTransform();
	const FBoneContainer& boneContainer = Output.Pose.GetPose().GetBoneContainer();

	// mirroring pose.
	for (auto target : Targets)
	{
		target.BoneRef.Initialize(boneContainer);
		if(target.BoneRef.IsValidToEvaluate()) {
			auto ai = target.BoneRef.GetCompactPoseIndex(boneContainer);
			auto at = Output.Pose.GetComponentSpaceTransform(ai);
			FAnimationRuntime::ConvertCSTransformToBoneSpace(componentTransform, Output.Pose, at, ai, BCS_BoneSpace);
			if (!target.CounterpartBoneRef.BoneName.IsNone())
			{
				target.CounterpartBoneRef.Initialize(boneContainer);
			}
			if (target.CounterpartBoneRef.IsValidToEvaluate())
			{
				auto bi = target.CounterpartBoneRef.GetCompactPoseIndex(boneContainer);
				auto bt = Output.Pose.GetComponentSpaceTransform(bi);
				FAnimationRuntime::ConvertCSTransformToBoneSpace(componentTransform, Output.Pose, bt, bi, BCS_BoneSpace);

				MirrorTransform(at, target.MirrorAxis);
				MirrorTransform(bt, target.MirrorAxis);

				FAnimationRuntime::ConvertBoneSpaceTransformToCS(componentTransform, Output.Pose, bt, ai, BCS_BoneSpace);
				FAnimationRuntime::ConvertBoneSpaceTransformToCS(componentTransform, Output.Pose, at, bi, BCS_BoneSpace);

				OutBoneTransforms.Add(FBoneTransform(ai, bt));
				OutBoneTransforms.Add(FBoneTransform(bi, at));
			}
			else
			{
				MirrorTransform(at, target.MirrorAxis);
				FAnimationRuntime::ConvertBoneSpaceTransformToCS(componentTransform, Output.Pose, at, ai, BCS_BoneSpace);
				OutBoneTransforms.Add(FBoneTransform(ai, at));
			}
		}
	}

	// for check in FCSPose<PoseType>::LocalBlendCSBoneTransforms
	OutBoneTransforms.Sort(FCompareBoneTransformIndex());
}


bool FAnimNode_Mirroring::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	return MirroringEnable;
}
