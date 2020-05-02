#include "AnimNode_Mirroring.h"
#include "AnimMirroringLog.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimationAsset.h"


namespace {
	inline FQuat HadamardProduct(const FQuat& q, const FVector4& v)
	{
		return FQuat(q.X * v.X, q.Y * v.Y, q.Z * v.Z, q.W * v.W);
	};

	inline void MirrorTransform(FTransform& InOutTransform, EMirroringAxis MirroringAxis)
	{
		switch(MirroringAxis) {
		case EMirroringAxis::XAxis:
			InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(-1.0f, 1.0f, 1.0f));
			InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(1.0f, -1.0f, -1.0f, 1.0f)));
			break;
		case EMirroringAxis::YAxis:
			InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(1.0f, -1.0f, 1.0f));
			InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(-1.0f, 1.0f, -1.0f, 1.0f)));
			break;
		case EMirroringAxis::ZAxis:
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

	UpdateTargets(boneContainer);

	UpdateCSRefPoseTransforms(boneContainer);

	InitializeBoneReferences(boneContainer);
}


void FAnimNode_Mirroring::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	check(OutBoneTransforms.Num() == 0);

	if (!MirroringEnable)
	{
		return;
	}

	FTransform componentTransform = Output.AnimInstanceProxy->GetComponentTransform();
	const FBoneContainer& boneContainer = Output.Pose.GetPose().GetBoneContainer();

	if(CSRefPoseTransforms.Num() == 0) UpdateCSRefPoseTransforms(boneContainer);

	if(Targets.Num() == 0
		&& (OverrideMirroringData.Num() > 0
			|| (MirroringData && (MirroringData->MirroringTargetDefines.Num() > 0 || MirroringData->DefaultMirroringAxis != EMirroringAxis::None))))
	{
		UpdateTargets(boneContainer);
	}

	if (Targets.Num() > 0 && !Targets[0].BoneRef.IsValidToEvaluate()) InitializeBoneReferences(boneContainer);

	// mirroring pose.
	for (auto& target : Targets)
	{
		if (target.BoneRef.IsValidToEvaluate())
		{
			auto ai = target.BoneRef.GetCompactPoseIndex(boneContainer);
			auto apt = CSRefPoseTransforms[ai.GetInt()];
			auto at = Output.Pose.GetComponentSpaceTransform(ai);

			if (target.CounterpartBoneRef.IsValidToEvaluate())
			{
				auto bi = target.CounterpartBoneRef.GetCompactPoseIndex(boneContainer);
				auto bpt = CSRefPoseTransforms[bi.GetInt()];
				auto bt = Output.Pose.GetComponentSpaceTransform(bi);

				at.SetRotation(at.GetRotation() * apt.GetRotation().Inverse());
				at.SetLocation(at.GetLocation() - apt.GetLocation());
				bt.SetRotation(bt.GetRotation() * bpt.GetRotation().Inverse());
				bt.SetLocation(bt.GetLocation() - bpt.GetLocation());

				MirrorTransform(at, target.MirrorAxis);
				MirrorTransform(bt, target.MirrorAxis);

				at.SetRotation(at.GetRotation() * bpt.GetRotation());
				at.SetLocation(at.GetLocation() + bpt.GetLocation());
				bt.SetRotation(bt.GetRotation() * apt.GetRotation());
				bt.SetLocation(bt.GetLocation() + apt.GetLocation());

				OutBoneTransforms.Add(FBoneTransform(ai, bt));
				OutBoneTransforms.Add(FBoneTransform(bi, at));
			}
			else
			{
				at.SetRotation(at.GetRotation() * apt.GetRotation().Inverse());
				at.SetLocation(at.GetLocation() - apt.GetLocation());

				MirrorTransform(at, target.MirrorAxis);
				
				at.SetRotation(at.GetRotation() * apt.GetRotation());
				at.SetLocation(at.GetLocation() + apt.GetLocation());

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


void FAnimNode_Mirroring::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	for (auto& target : Targets)
	{
		target.BoneRef.Initialize(RequiredBones);
		if (!target.CounterpartBoneRef.BoneName.IsNone())
		{
			target.CounterpartBoneRef.Initialize(RequiredBones);
		}
	}
}

void FAnimNode_Mirroring::UpdateCSRefPoseTransforms(const FBoneContainer& RequiredBones)
{
	CSRefPoseTransforms.Reset();
	CSRefPoseTransforms.Reserve(RequiredBones.GetCompactPoseNumBones());

	for(int32 iBone = 0; iBone < RequiredBones.GetCompactPoseNumBones(); iBone++) {
		FCompactPoseBoneIndex boneIndex(iBone);
		FTransform bonePose = RequiredBones.GetRefPoseTransform(boneIndex);
		FCompactPoseBoneIndex parentIndex = RequiredBones.GetParentBoneIndex(boneIndex);
		while(parentIndex.GetInt() != INDEX_NONE) {
			FTransform parentPose = RequiredBones.GetRefPoseTransform(parentIndex);
			bonePose = bonePose * parentPose;
			parentIndex = RequiredBones.GetParentBoneIndex(parentIndex);
		}
		CSRefPoseTransforms.Add(bonePose);
	}
}

void FAnimNode_Mirroring::Reset()
{
	Targets.Reset();
	CSRefPoseTransforms.Reset();
}

void FAnimNode_Mirroring::UpdateTargets(const FBoneContainer& RequiredBones)
{
	Targets.Reset();
	Targets.Reserve(RequiredBones.GetNumBones());

	if (OverrideMirroringData.Num() > 0 || (MirroringData && (MirroringData->MirroringTargetDefines.Num() > 0 || MirroringData->DefaultMirroringAxis != EMirroringAxis::None)))
	{
		for (int i = 0; i < RequiredBones.GetCompactPoseNumBones(); i++)
		{
			auto skeletonIndex = RequiredBones.GetSkeletonIndex(FCompactPoseBoneIndex(i));

			if (skeletonIndex >= RequiredBones.GetNumBones())
			{
				continue;
			}

			auto boneName = RequiredBones.GetReferenceSkeleton().GetBoneName(skeletonIndex);

			for (auto& j : Targets)
			{
				if(j.BoneRef.BoneName == boneName || (j.CounterpartBoneRef.BoneName.IsNone() && j.CounterpartBoneRef.BoneName == boneName)) continue;
			}

			FString counterBoneNameString;
			EMirroringAxis mirroringAxis;
			if(!FMirroringTargetDefine::FindMirroringAxis(OverrideMirroringData, boneName.ToString(), mirroringAxis, counterBoneNameString))
			{
				MirroringData->FindMirroringAxis(boneName.ToString(), mirroringAxis, counterBoneNameString);
			}

			if (mirroringAxis != EMirroringAxis::None)
			{
				FMirroringTarget target;
				target.MirrorAxis = mirroringAxis;

				target.BoneRef.BoneName = boneName;

				if (!counterBoneNameString.IsEmpty())
				{
					FName counterBoneName(*counterBoneNameString);
					if (RequiredBones.GetPoseBoneIndexForBoneName(counterBoneName) >= 0)
					{
						target.CounterpartBoneRef.BoneName = counterBoneName;
					}
				}
				Targets.Add(target);
			}
		}
	}
}
