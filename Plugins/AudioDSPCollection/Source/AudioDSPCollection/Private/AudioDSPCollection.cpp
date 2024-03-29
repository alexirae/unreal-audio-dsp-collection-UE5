#include "AudioDSPCollection.h"
#include "MetasoundEditorModule.h"
#include "MetasoundFrontendRegistries.h"

#define LOCTEXT_NAMESPACE "FAudioDSPCollectionModule"

void FAudioDSPCollectionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	Metasound::Editor::IMetasoundEditorModule& MetaSoundEditorModule = FModuleManager::GetModuleChecked<Metasound::Editor::IMetasoundEditorModule>("MetaSoundEditor");
	MetaSoundEditorModule.RegisterPinType("Enum:SaturationType", "Int32");

	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

void FAudioDSPCollectionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.	 For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAudioDSPCollectionModule, AudioDSPCollection)
