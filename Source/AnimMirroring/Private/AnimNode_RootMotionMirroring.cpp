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
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	Super::Initialize_AnyThread(Context);
	AssetPlayer.Initialize(Context);
}

void FAnimNode_RootMotionMirroring::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
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
	AssetPlayer.Update(Context);
}

void FAnimNode_RootMotionMirroring::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	AssetPlayer.Evaluate(Output);
}
