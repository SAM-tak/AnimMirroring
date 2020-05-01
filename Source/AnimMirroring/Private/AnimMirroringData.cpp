#include "AnimMirroringData.h"


FMirrorMatchData::FMirrorMatchData()
	: NameRule(EMirroringNameRule::ExactMatch)
	, BoneName()
	, CounterpartBoneName()
	, MirrorAxis(EMirrorAxis::None)
{

}


FMirrorMatchData::FMirrorMatchData(EMirroringNameRule InNameRule, const FString& InBoneName, EMirrorAxis InMirrorAxis)
	: NameRule(InNameRule)
	, BoneName(InBoneName)
	, CounterpartBoneName(InBoneName)
	, MirrorAxis(InMirrorAxis)
{

}


FMirrorMatchData::FMirrorMatchData(EMirroringNameRule InNameRule, const FString& InBoneName, const FString& InCounterpartBoneName, EMirrorAxis InMirrorAxis)
	: NameRule(InNameRule)
	, BoneName(InBoneName)
	, CounterpartBoneName(InCounterpartBoneName)
	, MirrorAxis(InMirrorAxis)
{

}


bool FMirrorMatchData::IsMatch(const FString& InBoneName)
{
	if(BoneName.IsEmpty()) return false;

	if (NameRule == EMirroringNameRule::HeadMatch)
	{
		return InBoneName.StartsWith(BoneName);
	}
	else if(NameRule == EMirroringNameRule::TailMatch)
	{
		return InBoneName.EndsWith(BoneName);
	}
	else
	{
		return InBoneName.Compare(BoneName, ESearchCase::IgnoreCase) == 0;
	}
}


bool FMirrorMatchData::IsMatchAsCounterpart(const FString& InBoneName)
{
	if(CounterpartBoneName.IsEmpty()) return false;

	if (NameRule == EMirroringNameRule::HeadMatch)
	{
		return InBoneName.StartsWith(CounterpartBoneName);
	}
	else if (NameRule == EMirroringNameRule::TailMatch)
	{
		return InBoneName.EndsWith(CounterpartBoneName);
	}
	else
	{
		return InBoneName.Compare(CounterpartBoneName, ESearchCase::IgnoreCase) == 0;
	}
}


FString FMirrorMatchData::GetCounterpartBoneName(const FString& InBoneName)
{
	if (NameRule == EMirroringNameRule::HeadMatch)
	{
		return CounterpartBoneName + InBoneName.RightChop(BoneName.Len());
	}
	else if (NameRule == EMirroringNameRule::TailMatch)
	{
		return InBoneName.LeftChop(BoneName.Len()) + CounterpartBoneName;
	}
	else if (NameRule == EMirroringNameRule::ExactMatch)
	{
		return CounterpartBoneName;
	}

	return "";
}


FString FMirrorMatchData::GetCounterCounterpartBoneName(const FString& InBoneName)
{
	if (NameRule == EMirroringNameRule::HeadMatch)
	{
		return BoneName + InBoneName.RightChop(CounterpartBoneName.Len());
	}
	else if (NameRule == EMirroringNameRule::TailMatch)
	{
		return InBoneName.LeftChop(CounterpartBoneName.Len()) + BoneName;
	}
	else if (NameRule == EMirroringNameRule::ExactMatch)
	{
		return BoneName;
	}

	return "";
}


EMirrorAxis FMirrorMatchData::FindMirrorAxis(const TArray<FMirrorMatchData>& MirrorMatches, const FString& InBoneName, FString& OutCounterpartBoneName)
{
	for (auto i : MirrorMatches)
	{
		if (i.IsMatch(InBoneName))
		{
			OutCounterpartBoneName = i.GetCounterpartBoneName(InBoneName);
			return i.MirrorAxis;
		}
		else if (i.IsMatchAsCounterpart(InBoneName))
		{
			OutCounterpartBoneName = i.GetCounterCounterpartBoneName(InBoneName);
			return i.MirrorAxis;
		}
	}
	OutCounterpartBoneName = "";
	return EMirrorAxis::None;
}


EMirrorAxis UAnimMirroringData::FindMirrorAxis(const FString& InBoneName, FString& OutCounterpartBoneName)
{
	auto retval = FMirrorMatchData::FindMirrorAxis(MirrorMatches, InBoneName, OutCounterpartBoneName);
	return retval == EMirrorAxis::None ? DefaultMirrorAxis : retval;
}


UAnimMirroringData::UAnimMirroringData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultMirrorAxis(EMirrorAxis::None)
{

}
