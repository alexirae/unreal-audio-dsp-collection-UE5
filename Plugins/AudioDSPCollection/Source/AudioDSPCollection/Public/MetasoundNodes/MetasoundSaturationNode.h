#include "DSPProcessing/Saturation.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	DECLARE_METASOUND_ENUM(DSPProcessing::ESaturationType, DSPProcessing::ESaturationType::Tape, AUDIODSPCOLLECTION_API, FEnumESaturationType, FEnumSaturationTypeInfo, FEnumSaturationReadRef, FEnumSaturationWriteRef);

	//------------------------------------------------------------------------------------
	// FSaturationOperator
	//------------------------------------------------------------------------------------
	class FSaturationOperator : public TExecutableOperator<FSaturationOperator>
	{
	public:
		FSaturationOperator(const FOperatorSettings& InSettings,
							const FAudioBufferReadRef& InAudioInput,
							const FFloatReadRef& InGain,
							const FFloatReadRef& InBias,
							const FFloatReadRef& InMix,
							const FFloatReadRef& InOutLevelDb,
							const FEnumSaturationReadRef& InSaturationType);

		static const FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override;
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

		void Execute();

	private:
		DSPProcessing::FSaturation SaturationDSPProcessor;

		FAudioBufferReadRef	 AudioInput;
		FAudioBufferWriteRef AudioOutput;

		FFloatReadRef		   Gain;
		FFloatReadRef		   Bias;
		FFloatReadRef		   Mix;
		FFloatReadRef		   OutLevelDb;
		FEnumSaturationReadRef SaturationType;
	};


	//------------------------------------------------------------------------------------
	// FSaturationNode
	//------------------------------------------------------------------------------------
	class FSaturationNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FSaturationNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSaturationOperator>())
		{

		}
	};
}
