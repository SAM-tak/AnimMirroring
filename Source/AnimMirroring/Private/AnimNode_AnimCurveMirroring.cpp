#include "AnimNode_AnimCurveMirroring.h"
#include "AnimMirroringLog.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

#include "Engine/Engine.h"

FAnimNode_AnimCurveMirroring::FAnimNode_AnimCurveMirroring() : FAnimNode_Base(), bEnabled(true)
{
	MirroringData.Add(FMirroringAnimCurveDefine(EMirroringMatchMode::TailMatch, "_L", "_R", false));
}

void FAnimNode_AnimCurveMirroring::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	Super::Initialize_AnyThread(Context);
	Source.Initialize(Context);
}

void FAnimNode_AnimCurveMirroring::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
	// Run update on input pose nodes
	Source.Update(Context);

	// Evaluate any BP logic plugged into this node
	GetEvaluateGraphExposedInputs().Execute(Context);
}

void FAnimNode_AnimCurveMirroring::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)

	if (bEnabled)
	{
		FPoseContext SourceData(Output);
		Source.Evaluate(SourceData);

		Output = SourceData;

		//	Morph target and Material parameter curves
		USkeleton* Skeleton = Output.AnimInstanceProxy->GetSkeleton();

		auto SmartNameContainer = Skeleton->GetSmartNameContainer(USkeleton::AnimCurveMappingName);

		auto maxUID = SmartNameContainer->GetMaxUID();

		ProccessedCurve.Reset(maxUID + 1);
		ProccessedCurve.AddZeroed(maxUID + 1);

		for (SmartName::UID_Type i = 0; i <= maxUID; i++)
		{
			FName name;
			if (SmartNameContainer->GetName(i, name))
			{
				if (ProccessedCurve[i])
					continue;

				auto CurveName = name.ToString();
				auto match = FMirroringAnimCurveDefine::Find(MirroringData, CurveName);
				if (match >= 0)
				{
					bool inverting = MirroringData[match].bValueInverting;
					auto CorrespondingName = MirroringData[match].GetCounterpartAnimCurveName(CurveName);
					for (SmartName::UID_Type j = 0; j <= maxUID; j++)
					{
						if (i == j || ProccessedCurve[j])
							continue;

						FName tname;
						SmartNameContainer->GetName(j, tname);
						auto TesteeCurveName = tname.ToString();
						if (TesteeCurveName == CorrespondingName)
						{
							auto lhs = Output.Curve.Get(i);
							auto rhs = Output.Curve.Get(j);
							Output.Curve.Set(i, inverting ? -rhs : rhs);
							Output.Curve.Set(j, inverting ? -lhs : lhs);
							UE_LOG(LogAnimMirroring, Log, TEXT("Swap %s (%f) <-> %s (%f)"), *CurveName, lhs, *CorrespondingName, rhs);
							ProccessedCurve[j] = true;
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		Source.Evaluate(Output);
	}
}
