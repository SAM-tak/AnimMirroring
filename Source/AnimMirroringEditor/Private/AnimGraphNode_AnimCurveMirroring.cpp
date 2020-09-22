#include "AnimGraphNode_AnimCurveMirroring.h"
#include "AnimationGraphSchema.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "FAnimMirroringEditorModule"

UAnimGraphNode_AnimCurveMirroring::UAnimGraphNode_AnimCurveMirroring(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UAnimGraphNode_AnimCurveMirroring::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimCurveMirroring", "Anim Curve Mirroring");
}

// Node Tooltip
FText UAnimGraphNode_AnimCurveMirroring::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_AnimCurveMirroring", "Mirroring Animation Curve Track by Name.");
}

FEditorModeID UAnimGraphNode_AnimCurveMirroring::GetEditorMode() const
{
	return "AnimGraph.SkeletalControl.Mirroring";
}

FString UAnimGraphNode_AnimCurveMirroring::GetNodeCategory() const
{
	return TEXT("Animation Curve Modifier");
}

#undef LOCTEXT_NAMESPACE