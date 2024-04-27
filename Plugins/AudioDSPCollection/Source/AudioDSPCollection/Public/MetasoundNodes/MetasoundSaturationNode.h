#include "DSPProcessing/Saturation.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	DECLARE_METASOUND_ENUM(DSPProcessing::ESaturationType, DSPProcessing::ESaturationType::Tape, AUDIODSPCOLLECTION_API, FEnumESaturationType, FEnumSaturationTypeInfo, FEnumSaturationReadRef, FEnumSaturationWriteRef);
}
	
namespace DSPCollection
{
	//------------------------------------------------------------------------------------
	// FSaturationOperator
	//------------------------------------------------------------------------------------
	class FSaturationOperator : public Metasound::TExecutableOperator<FSaturationOperator>
	{
	public:
		FSaturationOperator(const Metasound::FOperatorSettings& InSettings,
							const Metasound::FAudioBufferReadRef& InAudioInput,
							const Metasound::FFloatReadRef& InGain,
							const Metasound::FFloatReadRef& InBias,
							const Metasound::FFloatReadRef& InMix,
							const Metasound::FFloatReadRef& InOutLevelDb,
							const Metasound::FEnumSaturationReadRef& InSaturationType);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;
		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();

	private:
		DSPProcessing::FSaturation SaturationDSPProcessor;

		Metasound::FAudioBufferReadRef	AudioInput;
		Metasound::FAudioBufferWriteRef AudioOutput;

		Metasound::FFloatReadRef Gain;
		Metasound::FFloatReadRef Bias;
		Metasound::FFloatReadRef Mix;
		Metasound::FFloatReadRef OutLevelDb;
		Metasound::FEnumSaturationReadRef SaturationType;
	};


	//------------------------------------------------------------------------------------
	// FSaturationNode
	//------------------------------------------------------------------------------------
	class FSaturationNode : public Metasound::FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FSaturationNode(const Metasound::FNodeInitData& InitData)
			: Metasound::FNodeFacade(InitData.InstanceName, InitData.InstanceID, Metasound::TFacadeOperatorClass<FSaturationOperator>())
		{

		}
	};
}
