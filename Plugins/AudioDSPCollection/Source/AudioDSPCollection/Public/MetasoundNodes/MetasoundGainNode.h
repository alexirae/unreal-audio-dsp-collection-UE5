#include "DSPProcessing/Gain.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
	//------------------------------------------------------------------------------------
	// FGainOperator
	//------------------------------------------------------------------------------------
	class FGainOperator : public TExecutableOperator<FGainOperator>
	{
	public:
		FGainOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InGain);

		static const FNodeClassMetadata& GetNodeInfo();

		virtual FDataReferenceCollection GetInputs()  const override;
		virtual FDataReferenceCollection GetOutputs() const override;
		static const FVertexInterface& GetVertexInterface();
		static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

		void Execute();

	private:
		FAudioBufferReadRef	 AudioInput;
		FAudioBufferWriteRef AudioOutput;

		DSPProcessing::FGain GainDSPProcessor;

		FFloatReadRef Gain;
	};


	//------------------------------------------------------------------------------------
	// FGainNode
	//------------------------------------------------------------------------------------
	class FGainNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FGainNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FGainOperator>())
		{

		}
	};
}
