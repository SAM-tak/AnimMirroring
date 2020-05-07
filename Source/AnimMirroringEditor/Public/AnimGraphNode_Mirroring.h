#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_Mirroring.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_Mirroring.generated.h"

// AnimGraphNode
UCLASS(meta = (Keywords = "Mirror, Mirroring"))
class ANIMMIRRORINGEDITOR_API UAnimGraphNode_Mirroring : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_Mirroring Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	// UAnimGraphNode_Base interface
	virtual FEditorModeID GetEditorMode() const override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override
	{
		return &Node;
	}
	// End of UAnimGraphNode_SkeletalControlBase interface
};
