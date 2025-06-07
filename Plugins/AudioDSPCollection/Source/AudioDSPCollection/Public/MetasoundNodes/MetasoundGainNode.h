#include "DSPProcessing/Gain.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace DSPCollection
{
	class FGainOperator : public Metasound::TExecutableOperator<FGainOperator>
	{
	public:
		FGainOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudioInput, const Metasound::FFloatReadRef& InGain);

		static const Metasound::FNodeClassMetadata& GetNodeInfo();

		virtual void BindInputs(Metasound::FInputVertexInterfaceData& InOutVertexData) override;
		virtual void BindOutputs(Metasound::FOutputVertexInterfaceData& InOutVertexData) override;
		
		static const Metasound::FVertexInterface& GetVertexInterface();
		static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FBuildOperatorParams& InParams, Metasound::FBuildResults& OutResults);

		void Execute();
		
		void Reset(const IOperator::FResetParams& InParams);

	private:
		Metasound::FAudioBufferReadRef	AudioInput;
		Metasound::FAudioBufferWriteRef AudioOutput;

		DSPProcessing::FGain GainDSPProcessor;

		Metasound::FFloatReadRef Gain;
	};

	using FGainNode = Metasound::TNodeFacade<FGainOperator>;
}
