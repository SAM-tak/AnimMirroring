#include "AnimGraphNode_Mirroring.h"
#include "AnimationGraphSchema.h"

#define LOCTEXT_NAMESPACE "FAnimMirroringEditorModule"

UAnimGraphNode_Mirroring::UAnimGraphNode_Mirroring(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FText UAnimGraphNode_Mirroring::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

// Node Tooltip
FText UAnimGraphNode_Mirroring::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_Mirroring", "Mirroring Pose.");
}

FText UAnimGraphNode_Mirroring::GetControllerDescription() const
{
	return LOCTEXT("Mirroring", "Mirroring");
}

FEditorModeID UAnimGraphNode_Mirroring::GetEditorMode() const
{
	return "AnimGraph.SkeletalControl.Mirroring";
}

void UAnimGraphNode_Mirroring::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Node.Reset();
	ReconstructNode();
}

#undef LOCTEXT_NAMESPACE