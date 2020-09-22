#include "AnimMirroringData.h"

FMirroringTargetDefine::FMirroringTargetDefine()
	: MatchMode(EMirroringMatchMode::ExactMatch), BoneName(), CounterpartBoneName(), MirroringAxis(EMirroringAxis::None)
{
}

FMirroringTargetDefine::FMirroringTargetDefine(
	EMirroringMatchMode InNameRule, const FString& InBoneName, EMirroringAxis InMirrorAxis)
	: MatchMode(InNameRule), BoneName(InBoneName), CounterpartBoneName(InBoneName), MirroringAxis(InMirrorAxis)
{
}

FMirroringTargetDefine::FMirroringTargetDefine(
	EMirroringMatchMode InNameRule, const FString& InBoneName, const FString& InCounterpartBoneName, EMirroringAxis InMirrorAxis)
	: MatchMode(InNameRule), BoneName(InBoneName), CounterpartBoneName(InCounterpartBoneName), MirroringAxis(InMirrorAxis)
{
}

bool FMirroringTargetDefine::IsMatch(const FString& InBoneName) const
{
	if (BoneName.IsEmpty())
		return false;

	if (MatchMode == EMirroringMatchMode::HeadMatch)
	{
		return InBoneName.StartsWith(BoneName);
	}
	else if (MatchMode == EMirroringMatchMode::TailMatch)
	{
		return InBoneName.EndsWith(BoneName);
	}
	else
	{
		return InBoneName.Compare(BoneName, ESearchCase::IgnoreCase) == 0;
	}
}

bool FMirroringTargetDefine::IsMatchAsCounterpart(const FString& InBoneName) const
{
	if (CounterpartBoneName.IsEmpty())
		return false;

	if (MatchMode == EMirroringMatchMode::HeadMatch)
	{
		return InBoneName.StartsWith(CounterpartBoneName);
	}
	else if (MatchMode == EMirroringMatchMode::TailMatch)
	{
		return InBoneName.EndsWith(CounterpartBoneName);
	}
	else
	{
		return InBoneName.Compare(CounterpartBoneName, ESearchCase::IgnoreCase) == 0;
	}
}

FString FMirroringTargetDefine::GetCounterpartBoneName(const FString& InBoneName) const
{
	if (MatchMode == EMirroringMatchMode::HeadMatch)
	{
		return CounterpartBoneName + InBoneName.RightChop(BoneName.Len());
	}
	else if (MatchMode == EMirroringMatchMode::TailMatch)
	{
		return InBoneName.LeftChop(BoneName.Len()) + CounterpartBoneName;
	}
	else if (MatchMode == EMirroringMatchMode::ExactMatch)
	{
		return CounterpartBoneName;
	}

	return "";
}

FString FMirroringTargetDefine::GetCounterCounterpartBoneName(const FString& InBoneName) const
{
	if (MatchMode == EMirroringMatchMode::HeadMatch)
	{
		return BoneName + InBoneName.RightChop(CounterpartBoneName.Len());
	}
	else if (MatchMode == EMirroringMatchMode::TailMatch)
	{
		return InBoneName.LeftChop(CounterpartBoneName.Len()) + BoneName;
	}
	else if (MatchMode == EMirroringMatchMode::ExactMatch)
	{
		return BoneName;
	}

	return "";
}

bool FMirroringTargetDefine::FindMirroringAxis(const TArray<FMirroringTargetDefine>& MirrorMatches, const FString& InBoneName,
	EMirroringAxis& OutMirroringAxis, FString& OutCounterpartBoneName)
{
	for (auto i : MirrorMatches)
	{
		if (i.IsMatch(InBoneName))
		{
			OutCounterpartBoneName = i.GetCounterpartBoneName(InBoneName);
			OutMirroringAxis = i.MirroringAxis;
			return true;
		}
		else if (i.IsMatchAsCounterpart(InBoneName))
		{
			//OutCounterpartBoneName = i.GetCounterCounterpartBoneName(InBoneName);
			//OutMirroringAxis = i.MirroringAxis;
			OutCounterpartBoneName.Empty();
			OutMirroringAxis = EMirroringAxis::None;
			return true;
		}
	}
	OutCounterpartBoneName.Empty();
	OutMirroringAxis = EMirroringAxis::None;
	return false;
}

bool UAnimMirroringData::FindMirroringAxis(
	const FString& InBoneName, EMirroringAxis& OutMirroringAxis, FString& OutCounterpartBoneName) const
{
	if (FMirroringTargetDefine::FindMirroringAxis(MirroringTargetDefines, InBoneName, OutMirroringAxis, OutCounterpartBoneName))
		return true;
	else
	{
		OutMirroringAxis = DefaultMirroringAxis;
		return false;
	}
}

UAnimMirroringData::UAnimMirroringData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), DefaultMirroringAxis(EMirroringAxis::None)
{
}

FMirroringAnimCurveDefine::FMirroringAnimCurveDefine() : MatchMode(EMirroringMatchMode::ExactMatch)
{
}

FMirroringAnimCurveDefine::FMirroringAnimCurveDefine(
	EMirroringMatchMode InNameRule, const FString& InAnimCurveName, bool InValueInverting)
	: MatchMode(InNameRule), AnimCurveName(InAnimCurveName), bValueInverting(InValueInverting)
{
}

FMirroringAnimCurveDefine::FMirroringAnimCurveDefine(
	EMirroringMatchMode InNameRule, const FString& InAnimCurveName, const FString& InCounterpartAnimCurveName, bool InValueInverting)
	: MatchMode(InNameRule)
	, AnimCurveName(InAnimCurveName)
	, CounterpartAnimCurveName(InCounterpartAnimCurveName)
	, bValueInverting(InValueInverting)
{
}

bool FMirroringAnimCurveDefine::IsMatch(const FString& InAnimCurveName) const
{
	if (AnimCurveName.IsEmpty())
		return false;

	if (MatchMode == EMirroringMatchMode::HeadMatch)
	{
		return InAnimCurveName.StartsWith(AnimCurveName);
	}
	else if (MatchMode == EMirroringMatchMode::TailMatch)
	{
		return InAnimCurveName.EndsWith(AnimCurveName);
	}
	else
	{
		return InAnimCurveName.Compare(AnimCurveName, ESearchCase::IgnoreCase) == 0;
	}
}

FString FMirroringAnimCurveDefine::GetCounterpartAnimCurveName(const FString& InAnimCurveName) const
{
	if (MatchMode == EMirroringMatchMode::HeadMatch)
	{
		return CounterpartAnimCurveName + InAnimCurveName.RightChop(AnimCurveName.Len());
	}
	else if (MatchMode == EMirroringMatchMode::TailMatch)
	{
		return InAnimCurveName.LeftChop(AnimCurveName.Len()) + CounterpartAnimCurveName;
	}
	else if (MatchMode == EMirroringMatchMode::ExactMatch)
	{
		return CounterpartAnimCurveName;
	}

	return "";
}

int FMirroringAnimCurveDefine::Find(const TArray<FMirroringAnimCurveDefine>& InDefines, const FString& InAnimCurveName)
{
	for (int i = 0; i < InDefines.Num(); i++)
	{
		if (InDefines[i].IsMatch(InAnimCurveName))
		{
			return i;
		}
	}
	return -1;
}
