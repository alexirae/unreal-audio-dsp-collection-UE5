#include "MetasoundNodes/MetasoundSaturationNode.h"

#define LOCTEXT_NAMESPACE "DSPCollection_MetasoundSaturationNode"

namespace Metasound
{
	DEFINE_METASOUND_ENUM_BEGIN(DSPProcessing::ESaturationType, FEnumESaturationType, "SaturationType")
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Tape,              "TapeDescription",              "Tape",              "TapeTT",              "Tape Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Tape2,             "Tape2Description",             "Tape2",             "Tape2TT",             "Tape Saturation (HQ)"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Overdrive,         "OverdriveDescription",         "Overdrive",         "OverdriveTT",         "Overdrive Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Tube,              "TubeDescription",              "Tube",              "TubeTT",              "Tube Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Tube2,             "Tube2Description",             "Tube2",             "Tube2TT",             "Tube Saturation (HQ)"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Distortion,        "DistortionDescription",        "Distortion",        "DistortionTT",        "Distortion Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Metal,             "MetalDescription",             "Metal",             "MetalTT",             "Metal Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Fuzz,              "FuzzDescription",              "Fuzz",              "FuzzTT",              "Fuzz Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::HardClip,          "HardClipDescription",          "HardClip",          "HardClipTT",          "Hard Clip Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Foldback,          "FoldbackDescription",          "Foldback",          "FoldbackTT",          "Foldback Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::HalfWaveRectifier, "HalfWaveRectifierDescription", "HalfWaveRectifier", "HalfWaveRectifierTT", "Half Wave Rectifier Saturation"),
		DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::FullWaveRectifier, "FullWaveRectifierDescription", "FullWaveRectifier", "FullWaveRectifierTT", "Full Wave Rectifier Saturation")
	DEFINE_METASOUND_ENUM_END()
}

namespace DSPCollection
{
	using namespace Metasound;

	namespace SaturationNode
	{
		METASOUND_PARAM(InParamNameAudioInput,     "In",              "Audio input.")
		METASOUND_PARAM(InParamNameGain,           "Gain",            "The amount of gain to apply to the input signal. Range = [0.0, 100.0]")
		METASOUND_PARAM(InParamNameBias,           "Bias",            "The amount of DC bias to apply to the input signal, this generates even harmonics in the saturated signal. Range = [-1.0, 1.0]")
		METASOUND_PARAM(InParamNameMix,            "Mix",             "The amount of mix between the saturated signal and the direct input signal. Range = [0.0, 100.0]")
		METASOUND_PARAM(InParamNameOutLevelDb,     "Out Level (dB)",  "The amount of gain (in dB) to apply to the output signal. Range = [-96dB, +24dB]")
		METASOUND_PARAM(InParamNameSaturationType, "Saturation Type", "Saturation algorithm to use to process the audio.")
		METASOUND_PARAM(OutParamNameAudio,         "Out",             "Audio output.")
	}

	FSaturationOperator::FSaturationOperator(const FOperatorSettings& InSettings, 
											 const FAudioBufferReadRef& InAudioInput, 
											 const FFloatReadRef& InGain, 
											 const FFloatReadRef& InBias, 
											 const FFloatReadRef& InMix, 
											 const FFloatReadRef& InOutLevelDb,
											 const FEnumSaturationReadRef& InSaturationTypeType)
		: AudioInput(InAudioInput)
		, AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
		, Gain(InGain)
		, Bias(InBias)
		, Mix(InMix)
		, OutLevelDb(InOutLevelDb)
		, SaturationType(InSaturationTypeType)
	{
		SaturationDSPProcessor.Init();

		const float SampleRate            = InSettings.GetSampleRate();
		constexpr float SmoothingTimeInMs = 21.33f;

		SaturationDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
		SaturationDSPProcessor.InitBiasParam(SmoothingTimeInMs, SampleRate);
		SaturationDSPProcessor.InitMixParam(SmoothingTimeInMs, SampleRate);
		SaturationDSPProcessor.InitOutLevelParam(SmoothingTimeInMs, SampleRate);
	}

		const FNodeClassMetadata& FSaturationOperator::GetNodeInfo()
	{
		auto InitNodeInfo = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Info;

			Info.ClassName         = { TEXT("MetasoundDSPCollection"), TEXT("Saturation"), TEXT("Audio") };
			Info.MajorVersion      = 1;
			Info.MinorVersion      = 1;
			Info.DisplayName       = LOCTEXT("DSPCollection_SaturationDisplayName",     "Saturation");
			Info.Description       = LOCTEXT("DSPCollection_SaturationNodeDescription", "Applies saturation to the audio input.");
			Info.Author            = "Alex Perez";
			Info.PromptIfMissing   = PluginNodeMissingPrompt;
			Info.DefaultInterface  = GetVertexInterface();
			Info.CategoryHierarchy = { LOCTEXT("DSPCollection_SaturationNodeCategory", "Saturation") };

			return Info;
		};

		static const FNodeClassMetadata Info = InitNodeInfo();

		return Info;
	}

	void FSaturationOperator::BindInputs(FInputVertexInterfaceData& InOutVertexData)
	{
		using namespace SaturationNode;

		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameGain), Gain);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameBias), Bias);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameMix), Mix);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameOutLevelDb), OutLevelDb);
		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(InParamNameSaturationType), SaturationType);
	}

	void FSaturationOperator::BindOutputs(FOutputVertexInterfaceData& InOutVertexData)
	{
		using namespace SaturationNode;

		InOutVertexData.BindReadVertex(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);
	}

	const FVertexInterface& FSaturationOperator::GetVertexInterface()
	{
		using namespace SaturationNode;

		static const FVertexInterface Interface(
			FInputVertexInterface(
				TInputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameAudioInput)),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameGain),                          100.0f),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameBias),                          0.0f),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameMix),                           100.0f),
				TInputDataVertex<float>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameOutLevelDb),                    0.0f),
				TInputDataVertex<FEnumESaturationType>(METASOUND_GET_PARAM_NAME_AND_METADATA(InParamNameSaturationType), static_cast<int32>(DSPProcessing::ESaturationType::Tape))
			),
			
			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutParamNameAudio))
			)
		);

		return Interface;
	}

	TUniquePtr<IOperator> FSaturationOperator::CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults)
	{
		using namespace SaturationNode;

		FAudioBufferReadRef AudioIn = InParams.InputData.GetOrConstructDataReadReference<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), InParams.OperatorSettings);

		FFloatReadRef InGain       = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(METASOUND_GET_PARAM_NAME(InParamNameGain),       InParams.OperatorSettings);
		FFloatReadRef InBias       = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(METASOUND_GET_PARAM_NAME(InParamNameBias),       InParams.OperatorSettings);
		FFloatReadRef InMix        = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(METASOUND_GET_PARAM_NAME(InParamNameMix),        InParams.OperatorSettings);
		FFloatReadRef InOutLevelDb = InParams.InputData.GetOrCreateDefaultDataReadReference<float>(METASOUND_GET_PARAM_NAME(InParamNameOutLevelDb), InParams.OperatorSettings);

		FEnumSaturationReadRef InSaturationType = InParams.InputData.GetOrConstructDataReadReference<FEnumESaturationType>(METASOUND_GET_PARAM_NAME(InParamNameSaturationType));

		return MakeUnique<FSaturationOperator>(InParams.OperatorSettings, AudioIn, InGain, InBias, InMix, InOutLevelDb, InSaturationType);
	}

	void FSaturationOperator::Execute()
	{
		SaturationDSPProcessor.SetSaturationType(*SaturationType); // Set SaturationType first since Gain depends on it
		SaturationDSPProcessor.SetGain(*Gain);
		SaturationDSPProcessor.SetBias(*Bias);
		SaturationDSPProcessor.SetMix(*Mix);
		SaturationDSPProcessor.SetOutLevelDb(*OutLevelDb);

		const float* InputAudio = AudioInput->GetData();
		float* OutputAudio      = AudioOutput->GetData();
		const int32 NumSamples  = AudioInput->Num();

		SaturationDSPProcessor.ProcessAudioBuffer(InputAudio, OutputAudio, NumSamples);
	}

	METASOUND_REGISTER_NODE(FSaturationNode)
}

#undef LOCTEXT_NAMESPACE
