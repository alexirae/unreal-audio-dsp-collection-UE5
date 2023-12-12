#include "MetasoundNodes/MetasoundGainNode.h"

#define LOCTEXT_NAMESPACE "MetasoundDSPCollection_GainNode"

namespace Metasound
{
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
			Info.DisplayName       = LOCTEXT("Metasound_GainDisplayName", "Gain");
			Info.Description       = LOCTEXT("Metasound_GainNodeDescription", "Applies gain to the audio input.");
			Info.Author            = PluginAuthor;
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("Metasound_GainNodeCategory", "Utils") };

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

	TUniquePtr<IOperator> FGainOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
	{
		using namespace GainNode;

		const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
		const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

		FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput),					 InParams.OperatorSettings);
		FFloatReadRef InGain        = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameGain), InParams.OperatorSettings);

		return MakeUnique<FGainOperator>(InParams.OperatorSettings, AudioIn, InGain);
	}

	void FGainOperator::Execute()
	{
		const float* InputAudio = AudioInput->GetData();
		float* OutputAudio      = AudioOutput->GetData();

		const int32 NumSamples = AudioInput->Num();

		GainDSPProcessor.SetGain(*Gain);

		GainDSPProcessor.ProcessAudioBuffer(InputAudio, OutputAudio, NumSamples);
	}

	METASOUND_REGISTER_NODE(FGainNode)
}

#undef LOCTEXT_NAMESPACE
