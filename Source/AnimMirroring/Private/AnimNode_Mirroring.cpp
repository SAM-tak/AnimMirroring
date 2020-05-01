#include "AnimNode_Mirroring.h"
#include "AnimMirroringLog.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"


FAnimNode_Mirroring::FAnimNode_Mirroring()
	: FAnimNode_Base()
	, MirroringData(nullptr)
	, MirroringEnable(true)
{
}


void FAnimNode_Mirroring::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{
	ComponentPose.Initialize(Context);

	const FBoneContainer& BoneContainer = Context.AnimInstanceProxy->GetRequiredBones();

	ComponentSpaceRefPose.Reset();
	ComponentSpaceRefPose.Reserve(BoneContainer.GetCompactPoseNumBones());

	for (int32 iBone = 0; iBone < BoneContainer.GetCompactPoseNumBones(); iBone++) {
		FCompactPoseBoneIndex boneIndex(iBone);
		FTransform bonePose = BoneContainer.GetRefPoseTransform(boneIndex);

		FCompactPoseBoneIndex parentIndex = BoneContainer.GetParentBoneIndex(boneIndex);
		while (parentIndex.GetInt() != INDEX_NONE) {
			FTransform parentPose = BoneContainer.GetRefPoseTransform(parentIndex);
			bonePose = bonePose * parentPose;
			parentIndex = BoneContainer.GetParentBoneIndex(parentIndex);
		}
		ComponentSpaceRefPose.Add(bonePose);
	}

	MirrorInfo.Initialize(OverrideMirrorMatches, MirroringData, BoneContainer);
}


void FAnimNode_Mirroring::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	ComponentPose.CacheBones(Context);
}


void FAnimNode_Mirroring::Update_AnyThread(const FAnimationUpdateContext & Context)
{
	GetEvaluateGraphExposedInputs().Execute(Context);
	ComponentPose.Update(Context);
}


void FAnimNode_Mirroring::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
{
	Output.ResetToRefPose();
	ComponentPose.EvaluateComponentSpace(Output);
	if (!MirroringEnable) {
		return;
	}

	TArray<FTransform> CurrentTransforms;
	CurrentTransforms.Reserve(Output.Pose.GetPose().GetNumBones());

	// GetComponentSpaceTransform returns different value after SetComponentSpaceTransform called in mirror process loop.
	// Then, store all current component space poses before process.
	// ループ中にSetComponentSpaceTransformを使うと、GetComponentSpaceTransformの戻ってくる値が更新されてしまうので
	// 先にすべてのポーズを保管しておく。
	for (int32 iBone = 0; iBone < Output.Pose.GetPose().GetNumBones(); iBone++) {
		CurrentTransforms.Add(Output.Pose.GetComponentSpaceTransform(FCompactPoseBoneIndex(iBone)));
	}

	// mirroring pose.
	for (int32 iPair = 0; iPair < MirrorInfo.Items.Num(); iPair++) {
		_MirrorPose(Output, CurrentTransforms, MirrorInfo.Items[iPair]);
	}
}




void FAnimNode_Mirroring::_MirrorPose(FComponentSpacePoseContext& Output, const TArray<FTransform>& CurrentTransforms, const FMirrorInfoItem& Item)
{
	if (Item.IndexA >= 0 && Item.IndexA >= 0) {
		if (Item.IndexA != Item.IndexB) {
			// Pair bone mirroring
			FTransform btA = CurrentTransforms[Item.IndexA];
			FTransform btB = CurrentTransforms[Item.IndexB];

			btA.SetRotation(btA.GetRotation() * ComponentSpaceRefPose[Item.IndexA].GetRotation().Inverse());
			btA.SetLocation(btA.GetLocation() - ComponentSpaceRefPose[Item.IndexA].GetLocation());
			btB.SetRotation(btB.GetRotation() * ComponentSpaceRefPose[Item.IndexB].GetRotation().Inverse());
			btB.SetLocation(btB.GetLocation() - ComponentSpaceRefPose[Item.IndexB].GetLocation());

			FMirrorInfo::MirrorTransform(btA, Item.MirrorAxis);
			FMirrorInfo::MirrorTransform(btB, Item.MirrorAxis);

			Output.Pose.SetComponentSpaceTransform(FCompactPoseBoneIndex(Item.IndexA), FTransform(
				btB.GetRotation() * ComponentSpaceRefPose[Item.IndexA].GetRotation(),
				btB.GetTranslation() + ComponentSpaceRefPose[Item.IndexA].GetLocation(),
				btB.GetScale3D()
			));
			Output.Pose.SetComponentSpaceTransform(FCompactPoseBoneIndex(Item.IndexB), FTransform(
				btA.GetRotation() * ComponentSpaceRefPose[Item.IndexB].GetRotation(),
				btA.GetTranslation() + ComponentSpaceRefPose[Item.IndexB].GetLocation(),
				btA.GetScale3D()
			));
		}
		else {
			// Single bone mirroring
			FTransform btA = CurrentTransforms[Item.IndexA];

			btA.SetRotation(btA.GetRotation() * ComponentSpaceRefPose[Item.IndexA].GetRotation().Inverse());
			btA.SetLocation(btA.GetLocation() - ComponentSpaceRefPose[Item.IndexA].GetLocation());

			FMirrorInfo::MirrorTransform(btA, Item.MirrorAxis);

			Output.Pose.SetComponentSpaceTransform(FCompactPoseBoneIndex(Item.IndexA), FTransform(
				btA.GetRotation() * ComponentSpaceRefPose[Item.IndexA].GetRotation(),
				btA.GetTranslation() + ComponentSpaceRefPose[Item.IndexA].GetLocation(),
				btA.GetScale3D()
			));
		}
	}
}
