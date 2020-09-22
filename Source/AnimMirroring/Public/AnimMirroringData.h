#pragma once

#include "CoreMinimal.h"
#include "AnimMirroringData.generated.h"

/** ミラーリングの軸 */
UENUM()
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
UENUM()
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

	bool IsMatch(const FString& InBoneName) const;
	bool IsMatchAsCounterpart(const FString& InBoneName) const;
	FString GetCounterpartBoneName(const FString& BoneName) const;
	FString GetCounterCounterpartBoneName(const FString& BoneName) const;

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

	bool FindMirroringAxis(const FString& InBoneName, EMirroringAxis& OutMirroringAxis, FString& OutCounterpartBoneName) const;

    static inline FQuat HadamardProduct(const FQuat& q, const FVector4& v)
	{
		return FQuat(q.X * v.X, q.Y * v.Y, q.Z * v.Z, q.W * v.W);
	};

	static inline void MirrorTransform(FTransform& InOutTransform, EMirroringAxis MirroringAxis)
	{
		switch (MirroringAxis)
		{
			case EMirroringAxis::XAxis:
				InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(-1.0f, 1.0f, 1.0f));
				InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(1.0f, -1.0f, -1.0f, 1.0f)));
				break;
			case EMirroringAxis::YAxis:
				InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(1.0f, -1.0f, 1.0f));
				InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(-1.0f, 1.0f, -1.0f, 1.0f)));
				break;
			case EMirroringAxis::ZAxis:
				InOutTransform.SetLocation(InOutTransform.GetLocation() * FVector(1.0f, 1.0f, -1.0f));
				InOutTransform.SetRotation(HadamardProduct(InOutTransform.GetRotation(), FVector4(-1.0f, -1.0f, 1.0f, 1.0f)));
				break;
		}
	}
};

/** ミラーリングするアニメーションカーブ情報 */
USTRUCT(BlueprintType)
struct ANIMMIRRORING_API FMirroringAnimCurveDefine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ボーン名の指定規則
	EMirroringMatchMode MatchMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// 対象カーブの名前
	FString AnimCurveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// ミラーリングする時の対になるカーブの名前
	FString CounterpartAnimCurveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// 値もミラーリングするか？
	bool bValueInverting;

	FMirroringAnimCurveDefine();
	FMirroringAnimCurveDefine(EMirroringMatchMode InNameRule, const FString& InAnimCurveName, bool ValueInverting);
	FMirroringAnimCurveDefine(
		EMirroringMatchMode InNameRule, const FString& InAnimCurveName, const FString& InCounterpartAnimCurveName, bool ValueInverting);

	bool IsMatch(const FString& InAnimCurveName) const;
	FString GetCounterpartAnimCurveName(const FString& InAnimCurveName) const;

	static int Find(const TArray<FMirroringAnimCurveDefine>& InDefines, const FString& InAnimCurveName);
};
