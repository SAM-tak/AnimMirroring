#include "AnimMirroringDataFactory.h"
#include "AnimMirroringData.h"

UAnimMirroringDataFactory::UAnimMirroringDataFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UAnimMirroringData::StaticClass();
	bCreateNew = true;
}

bool UAnimMirroringDataFactory::DoesSupportClass(UClass* Class)
{
	return (Class == UAnimMirroringData::StaticClass());
}

UClass* UAnimMirroringDataFactory::ResolveSupportedClass()
{
	return UAnimMirroringData::StaticClass();
}

UObject* UAnimMirroringDataFactory::FactoryCreateNew(
	UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UAnimMirroringData* MirroringData = NewObject<UAnimMirroringData>(InParent, InName, Flags);

	if (!MirroringData)
	{
		return nullptr;
	}

	MirroringData->DefaultMirroringAxis = EMirroringAxis::XAxis;
	MirroringData->MirroringTargetDefines.Add(
		FMirroringTargetDefine(EMirroringMatchMode::TailMatch, FString("_l"), FString("_r"), EMirroringAxis::XAxis));

	return MirroringData;
}