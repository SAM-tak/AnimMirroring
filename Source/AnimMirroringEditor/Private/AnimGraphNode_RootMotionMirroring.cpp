#include "AnimGraphNode_RootMotionMirroring.h"
#include "AnimGraphNode_AssetPlayerBase.h"
#include "AnimationGraphSchema.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "FAnimMirroringEditorModule"

UAnimGraphNode_RootMotionMirroring::UAnimGraphNode_RootMotionMirroring(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UAnimGraphNode_RootMotionMirroring::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("RootMotionMirroring", "Root Motion Mirroring");
}

// Node Tooltip
FText UAnimGraphNode_RootMotionMirroring::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_RootMotionMirroring", "Mirroring Root Motion.");
}

FEditorModeID UAnimGraphNode_RootMotionMirroring::GetEditorMode() const
{
	return "AnimGraph.SkeletalControl.Mirroring";
}

FString UAnimGraphNode_RootMotionMirroring::GetNodeCategory() const
{
	return TEXT("Root Motion Modifier");
}

void UAnimGraphNode_RootMotionMirroring::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	if (IsPinExposedAndLinked(TEXT("AssetPlayer")))
	{
		auto pin = FindPin(TEXT("AssetPlayer"));
		auto node = pin->LinkedTo[0]->GetOwningNode();
		auto casted = Cast<UAnimGraphNode_AssetPlayerBase>(node);
		if (!casted)
		{
			MessageLog.Error(
				TEXT("@@ reference non-asset palayer node. this node effects when link to asset player node directly."), this);
		}
	}
}

#undef LOCTEXT_NAMESPACE