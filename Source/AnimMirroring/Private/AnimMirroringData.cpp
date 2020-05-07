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

bool FMirroringTargetDefine::IsMatch(const FString& InBoneName)
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

bool FMirroringTargetDefine::IsMatchAsCounterpart(const FString& InBoneName)
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

FString FMirroringTargetDefine::GetCounterpartBoneName(const FString& InBoneName)
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

FString FMirroringTargetDefine::GetCounterCounterpartBoneName(const FString& InBoneName)
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
			OutCounterpartBoneName = i.GetCounterCounterpartBoneName(InBoneName);
			OutMirroringAxis = i.MirroringAxis;
			return true;
		}
	}
	OutCounterpartBoneName = "";
	OutMirroringAxis = EMirroringAxis::None;
	return false;
}

bool UAnimMirroringData::FindMirroringAxis(
	const FString& InBoneName, EMirroringAxis& OutMirroringAxis, FString& OutCounterpartBoneName)
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
