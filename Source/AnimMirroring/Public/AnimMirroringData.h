#pragma once

#include "CoreMinimal.h"
#include "AnimMirroringData.generated.h"


/** ミラーリングの軸 */
UENUM(BlueprintType)
enum class EMirrorAxis : uint8
{
	None,
	XAxis,
	YAxis,
	ZAxis,
};


/** ミラーリングするボーン名の指定規則 */
UENUM(BlueprintType)
enum class EMirroringNameRule : uint8
{
	// 常に不一致
	Never,
	// 常に一致
	Always,
	// 完全一致
	ExactMatch,
	// 前方一致
	HeadMatch,
	// 後方一致
	TailMatch
};


/** ミラーリングするポーン情報 */
USTRUCT(BlueprintType)
struct ANIMMIRRORING_API FMirrorMatchData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ボーン名の指定規則
	EMirroringNameRule NameRule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// 対象ボーンの名前
	FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ミラーリングする時の対になるボーンの名前(省略した場合は単独でミラーリングする)
	FString PairBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ミラーリングの軸
	EMirrorAxis MirrorAxis;

	FMirrorMatchData();
	FMirrorMatchData(EMirroringNameRule InNameRule, const FString& InBoneName, EMirrorAxis InMirrorAxis);
	FMirrorMatchData(EMirroringNameRule InNameRule, const FString& InBoneName, const FString& InPairBoneName, EMirrorAxis InMirrorAxis);
};


/** ミラーリング用データ */
UCLASS()
class ANIMMIRRORING_API UAnimMirroringData : public UObject
{
	GENERATED_BODY()

public:
	UAnimMirroringData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMirrorAxis DefaultMirrorAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMirrorMatchData> MirrorMatches;
};
