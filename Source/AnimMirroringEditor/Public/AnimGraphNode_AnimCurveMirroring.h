#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_AnimCurveMirroring.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_AnimCurveMirroring.generated.h"

class FCompilerResultsLog;

// AnimGraphNode
UCLASS(meta = (Keywords = "Mirror, Mirroring"))
class ANIMMIRRORINGEDITOR_API UAnimGraphNode_AnimCurveMirroring : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_AnimCurveMirroring Node;

	// UAnimGraphNode_Base interface
	virtual FEditorModeID GetEditorMode() const override;
	virtual FString GetNodeCategory() const override;
	// End of UAnimGraphNode_Base interface

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface
};
