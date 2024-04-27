#include "MetasoundNodes/MetasoundGainNode.h"

#define LOCTEXT_NAMESPACE "DSPCollection_MetasoundGainNode"

namespace DSPCollection
{
	using namespace Metasound;

	namespace GainNode
	{
		METASOUND_PARAM(InParamNameAudioInput, "In",   "Audio input.")
		METASOUND_PARAM(InParamNameGain,       "Gain", "The amount of gain to apply to the input signal. Range = [0.0, 1.0]")
		METASOUND_PARAM(OutParamNameAudio,     "Out",  "Audio output.")
	}

	FGainOperator::FGainOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InGain)
		: AudioInput(InAudioInput)
		, AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
		, Gain(InGain)
	{
		const float SampleRate            = InSettings.GetSampleRate();
		constexpr float SmoothingTimeInMs = 21.33f;

		GainDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
	}

	const FNodeClassMetadata& FGainOperator::GetNodeInfo()
	{
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("MetasoundDSPCollection"), TEXT("Gain"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 1;
			Info.DisplayName       = LOCTEXT("DSPCollection_GainDisplayName",     "Gain");
			Info.Description       = LOCTEXT("DSPCollection_GainNodeDescription", "Applies gain to the audio input.");
			Info.Author            = "Alex Perez";
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("DSPCollection_GainNodeCategory", "Saturation") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

    void FGainOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData)
	{
		using namespace GainNode;

        InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameGain), Gain);
	}

    void FGainOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData)
	{
		using namespace GainNode;

        InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);
	}

	const FVertexInterface& FGainOperator::GetVertexInterface()
	{
		using namespace GainNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput)),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameGain), 1.0f)
			),

			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FGainOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults)
	{
		using namespace GainNode;

		FAudioBufferReadRef AudioIn = InParams.InputData.GetOrConstructDataReadReference<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);
		FFloatReadRef InGain        = InParams.InputData.GetOrCreateDefaultDataReadReference<float>   (METASOUND_GET_PARAM_NAME(InParamNameGain),       InParams.OperatorSettings);

		return MakeUnique<FGainOperator>(InParams.OperatorSettings, AudioIn, InGain);
	}

	void FGainOperator::Execute()
	{
		GainDSPProcessor.SetGain(*Gain);

		const float* InputAudio = AudioInput->GetData();
		float* OutputAudio      = AudioOutput->GetData();
		const int32 NumSamples  = AudioInput->Num();

		GainDSPProcessor.ProcessAudioBuffer(InputAudio, OutputAudio, NumSamples);
	}

	METASOUND_REGISTER_NODE(FGainNode)
}

#undef LOCTEXT_NAMESPACE
