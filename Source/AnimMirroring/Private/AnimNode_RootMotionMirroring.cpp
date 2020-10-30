#include "AnimNode_RootMotionMirroring.h"
#include "AnimMirroringLog.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimNode_AssetPlayerBase.h"

#include "Engine/Engine.h"
#include "Hash/CityHash.h"

FAnimNode_RootMotionMirroring::FAnimNode_RootMotionMirroring() : FAnimNode_Base(), Axis(EMirroringAxis::XAxis), bEnabled(true)
{
}

void FAnimNode_RootMotionMirroring::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	AssetPlayer.Initialize(Context);
}

void FAnimNode_RootMotionMirroring::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	FAnimNode_Base::Update_AnyThread(Context);
	AssetPlayer.Update(Context);

	if (IsLODEnabled(Context.AnimInstanceProxy))
	{
		GetEvaluateGraphExposedInputs().Execute(Context);

		if (bEnabled && Axis != EMirroringAxis::None && AssetPlayer.GetLinkNode())
		{
			auto axis = Axis;
			auto assetPlayer = (FAnimNode_AssetPlayerBase*) AssetPlayer.GetLinkNode();
			assetPlayer->ModifyRootMotionDelegate.BindLambda([axis](FTransform& Out) {
				UAnimMirroringData::MirrorTransform(Out, axis);
			});
		}
	}
}

void FAnimNode_RootMotionMirroring::Evaluate_AnyThread(FPoseContext& Output)
{
	AssetPlayer.Evaluate(Output);
}
