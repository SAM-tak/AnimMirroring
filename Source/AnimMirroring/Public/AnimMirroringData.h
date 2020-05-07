#pragma once

#include "CoreMinimal.h"
#include "AnimMirroringData.generated.h"

/** ミラーリングの軸 */
UENUM(BlueprintType)
enum class EMirroringAxis : uint8
{
	// ミラーリングしない
	None,
	// X軸でミラーリング
	XAxis,
	// Y軸でミラーリング
	YAxis,
	// Z軸でミラーリング
	ZAxis,
};

/** ボーン名の指定規則 */
UENUM(BlueprintType)
enum class EMirroringMatchMode : uint8
{
	// 完全一致
	ExactMatch,
	// 前方一致
	HeadMatch,
	// 後方一致
	TailMatch
};

/** ミラーリングするポーン情報 */
USTRUCT(BlueprintType)
struct ANIMMIRRORING_API FMirroringTargetDefine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ボーン名の指定規則
	EMirroringMatchMode MatchMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// 対象ボーンの名前
	FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ミラーリングする時の対になるボーンの名前(省略した場合は単独でミラーリングする)
	FString CounterpartBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ミラーリングの軸
	EMirroringAxis MirroringAxis;

	FMirroringTargetDefine();
	FMirroringTargetDefine(EMirroringMatchMode InNameRule, const FString& InBoneName, EMirroringAxis InMirrorAxis);
	FMirroringTargetDefine(EMirroringMatchMode InNameRule, const FString& InBoneName, const FString& InCounterpartBoneName,
		EMirroringAxis InMirrorAxis);

	bool IsMatch(const FString& InBoneName);
	bool IsMatchAsCounterpart(const FString& InBoneName);
	FString GetCounterpartBoneName(const FString& BoneName);
	FString GetCounterCounterpartBoneName(const FString& BoneName);

	static bool FindMirroringAxis(const TArray<FMirroringTargetDefine>& MirroringTargetDef, const FString& InBoneName,
		EMirroringAxis& OutMirroringAxis, FString& OutCounterpartBoneName);
};

/** ミラーリング用データ */
UCLASS()
class ANIMMIRRORING_API UAnimMirroringData : public UObject
{
	GENERATED_BODY()

public:
	UAnimMirroringData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMirroringAxis DefaultMirroringAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMirroringTargetDefine> MirroringTargetDefines;

	bool FindMirroringAxis(const FString& InBoneName, EMirroringAxis& OutMirroringAxis, FString& OutCounterpartBoneName);
};
