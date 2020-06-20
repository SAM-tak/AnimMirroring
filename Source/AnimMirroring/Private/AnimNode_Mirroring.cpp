#include "AnimNode_Mirroring.h"
#include "AnimMirroringLog.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimationAsset.h"

#include "Engine/Engine.h"
#include "Hash/CityHash.h"

FAnimNode_Mirroring::FAnimNode_Mirroring() : FAnimNode_SkeletalControlBase(), MirroringData(nullptr)
{
}

void FAnimNode_Mirroring::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);

	const FBoneContainer& boneContainer = Context.AnimInstanceProxy->GetRequiredBones();

	UpdateTargets(boneContainer);

	InitializeBoneReferences(boneContainer);
}

DECLARE_CYCLE_STAT(TEXT("AnimNode_Mirroring_Eval"), STAT_AnimNode_Mirroring_Eval, STATGROUP_Anim);

void FAnimNode_Mirroring::EvaluateSkeletalControl_AnyThread(
	FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	SCOPE_CYCLE_COUNTER(STAT_AnimNode_Mirroring_Eval);

	check(OutBoneTransforms.Num() == 0);

	const FBoneContainer& boneContainer = Output.Pose.GetPose().GetBoneContainer();

	UpdateIfNeeds(boneContainer);

	// mirroring pose.
	for (auto& target : Targets)
	{
		if (target.BoneRef.IsValidToEvaluate())
		{
			auto ai = target.BoneRef.GetCompactPoseIndex(boneContainer);
			auto apt = target.CSRefPose;
			auto at = Output.Pose.GetComponentSpaceTransform(ai);

			if (target.CounterpartBoneRef.IsValidToEvaluate())
			{
				auto bi = target.CounterpartBoneRef.GetCompactPoseIndex(boneContainer);
				auto bpt = target.CounterpartCSRefPose;
				auto bt = Output.Pose.GetComponentSpaceTransform(bi);

				at.SetRotation(at.GetRotation() * apt.GetRotation().Inverse());
				at.SetLocation(at.GetLocation() - apt.GetLocation());
				bt.SetRotation(bt.GetRotation() * bpt.GetRotation().Inverse());
				bt.SetLocation(bt.GetLocation() - bpt.GetLocation());

				UAnimMirroringData::MirrorTransform(at, target.Axis);
				UAnimMirroringData::MirrorTransform(bt, target.Axis);

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

				UAnimMirroringData::MirrorTransform(at, target.Axis);

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
	return Alpha > 0;
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

void FAnimNode_Mirroring::Reset()
{
	Targets.Reset();
}

void FAnimNode_Mirroring::UpdateTargets(const FBoneContainer& RequiredBones)
{
	Targets.Reset();

	if (OverrideMirroringData.Num() > 0 || (MirroringData && (MirroringData->MirroringTargetDefines.Num() > 0 ||
																 MirroringData->DefaultMirroringAxis != EMirroringAxis::None)))
	{
		for (int i = 0; i < RequiredBones.GetNumBones(); i++)
		{
			auto boneName = RequiredBones.GetReferenceSkeleton().GetBoneName(i);

			bool skip = false;
			for (auto& j : Targets)
			{
				if (j.BoneRef.BoneName == boneName ||
					(!j.CounterpartBoneRef.BoneName.IsNone() && j.CounterpartBoneRef.BoneName == boneName))
				{
					skip = true;
					break;
				}
			}
			if (skip)
				continue;

			FString counterpartBoneNameString;
			EMirroringAxis mirroringAxis;
			if (!FMirroringTargetDefine::FindMirroringAxis(
					OverrideMirroringData, boneName.ToString(), mirroringAxis, counterpartBoneNameString))
			{
				MirroringData->FindMirroringAxis(boneName.ToString(), mirroringAxis, counterpartBoneNameString);
			}

			if (mirroringAxis != EMirroringAxis::None)
			{
				FMirroringTarget target;
				target.Axis = mirroringAxis;

				target.BoneRef.BoneName = boneName;

				FTransform bonePose = RequiredBones.GetReferenceSkeleton().GetRefBonePose()[i];
				int parentIndex = RequiredBones.GetParentBoneIndex(i);

				while (parentIndex != INDEX_NONE)
				{
					FTransform parentPose = RequiredBones.GetReferenceSkeleton().GetRefBonePose()[parentIndex];
					bonePose = bonePose * parentPose;
					parentIndex = RequiredBones.GetParentBoneIndex(parentIndex);
				}

				target.CSRefPose = bonePose;

				if (!counterpartBoneNameString.IsEmpty())
				{
					FName counterpartBoneName(*counterpartBoneNameString);
					int j = RequiredBones.GetPoseBoneIndexForBoneName(counterpartBoneName);
					if (j != INDEX_NONE)
					{
						target.CounterpartBoneRef.BoneName = counterpartBoneName;

						bonePose = RequiredBones.GetReferenceSkeleton().GetRefBonePose()[j];
						parentIndex = RequiredBones.GetParentBoneIndex(j);

						while (parentIndex != INDEX_NONE)
						{
							FTransform parentPose = RequiredBones.GetReferenceSkeleton().GetRefBonePose()[parentIndex];
							bonePose = bonePose * parentPose;
							parentIndex = RequiredBones.GetParentBoneIndex(parentIndex);
						}

						target.CounterpartCSRefPose = bonePose;
					}
				}
				Targets.Add(target);
			}
		}
	}
}

void FAnimNode_Mirroring::UpdateIfNeeds(const FBoneContainer& RequiredBones)
{
	if (Targets.Num() == 0 &&
		(OverrideMirroringData.Num() > 0 || (MirroringData && (MirroringData->MirroringTargetDefines.Num() > 0 ||
																  MirroringData->DefaultMirroringAxis != EMirroringAxis::None))))
	{
		UpdateTargets(RequiredBones);
	}

	if (Targets.Num() > 0 && !Targets[0].BoneRef.IsValidToEvaluate())
		InitializeBoneReferences(RequiredBones);
}
