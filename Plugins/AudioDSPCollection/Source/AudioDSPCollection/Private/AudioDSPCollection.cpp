#include "AudioDSPCollection.h"

#include "MetasoundFrontendModuleRegistrationMacros.h"

#if WITH_EDITOR
#include "MetasoundEditorModule.h"
#endif

#define LOCTEXT_NAMESPACE "FAudioDSPCollectionModule"

namespace FAudioDSPCollectionModulePrivate
{
	const FName SaturationTypePinDataTypeName(TEXT("Enum:SaturationType"));
}

void FAudioDSPCollectionModule::StartupModule()
{
	using namespace FAudioDSPCollectionModulePrivate;

	METASOUND_REGISTER_ITEMS_IN_MODULE

#if WITH_EDITOR
	Metasound::Editor::IMetasoundEditorModule& MetaSoundEditorModule = FModuleManager::LoadModuleChecked<Metasound::Editor::IMetasoundEditorModule>(Metasound::Editor::IMetasoundEditorModule::ModuleName);

	Metasound::Editor::FGraphPinParams PinParams;
	PinParams.PinCategory = TEXT("Int32");

	MetaSoundEditorModule.GetGraphPanelPinFactory()->RegisterPin(SaturationTypePinDataTypeName, PinParams);
#endif
}

void FAudioDSPCollectionModule::ShutdownModule()
{
	using namespace FAudioDSPCollectionModulePrivate;

#if WITH_EDITOR
	if (FModuleManager::Get().IsModuleLoaded(Metasound::Editor::IMetasoundEditorModule::ModuleName))
	{
		Metasound::Editor::IMetasoundEditorModule& MetaSoundEditorModule = FModuleManager::GetModuleChecked<Metasound::Editor::IMetasoundEditorModule>(Metasound::Editor::IMetasoundEditorModule::ModuleName);

		MetaSoundEditorModule.GetGraphPanelPinFactory()->UnregisterPin(SaturationTypePinDataTypeName);
	}
#endif

	METASOUND_UNREGISTER_ITEMS_IN_MODULE
}

#undef LOCTEXT_NAMESPACE

METASOUND_IMPLEMENT_MODULE_REGISTRATION_LIST

IMPLEMENT_MODULE(FAudioDSPCollectionModule, AudioDSPCollection)
