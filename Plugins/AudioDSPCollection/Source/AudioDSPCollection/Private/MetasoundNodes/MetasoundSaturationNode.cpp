#include "MetasoundNodes/MetasoundSaturationNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SaturationNode"


METASOUND_REGISTER_NODE(FSaturationNode)


//------------------------------------------------------------------------------------
// FSaturationOperator
//------------------------------------------------------------------------------------
FSaturationOperator::FSaturationOperator(const Metasound::FOperatorSettings& InSettings, 
                                         const Metasound::FAudioBufferReadRef& InAudioInput, 
                                         const FEnumSaturationReadRef& InSaturationTypeType,
                                         const Metasound::FFloatReadRef& InGain, 
                                         const Metasound::FFloatReadRef& InBias, 
                                         const Metasound::FFloatReadRef& InMix, 
                                         const Metasound::FFloatReadRef& InOutLevelDb)
    : AudioInput(InAudioInput)
    , AudioOutput(Metasound::FAudioBufferWriteRef::CreateNew(InSettings))
    , SaturationType(InSaturationTypeType)
    , Gain(InGain)
    , Bias(InBias)
    , Mix(InMix)
    , OutLevelDb(InOutLevelDb)
{
    SaturationDSPProcessor.Init();

    const float SampleRate            = InSettings.GetSampleRate();
    constexpr float SmoothingTimeInMs = 21.33f;

    SaturationDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
    SaturationDSPProcessor.InitBiasParam(SmoothingTimeInMs, SampleRate);
    SaturationDSPProcessor.InitMixParam(SmoothingTimeInMs, SampleRate);
    SaturationDSPProcessor.InitOutLevelParam(SmoothingTimeInMs, SampleRate);
}

Metasound::FDataReferenceCollection FSaturationOperator::GetInputs() const
{
    Metasound::FDataReferenceCollection InputDataReferences;

    InputDataReferences.AddDataReadReference(SaturationNode::InParamNameAudioInput, AudioInput);
    InputDataReferences.AddDataReadReference(SaturationNode::InParamNameGain, Gain);
    InputDataReferences.AddDataReadReference(SaturationNode::InParamNameBias, Bias);
    InputDataReferences.AddDataReadReference(SaturationNode::InParamNameMix, Mix);
    InputDataReferences.AddDataReadReference(SaturationNode::InParamNameOutLevelDb, OutLevelDb);
    InputDataReferences.AddDataReadReference(SaturationNode::InParamNameSaturationType, SaturationType);

    return InputDataReferences;
}

Metasound::FDataReferenceCollection FSaturationOperator::GetOutputs() const
{
    Metasound::FDataReferenceCollection OutputDataReferences;

    OutputDataReferences.AddDataReadReference(SaturationNode::OutParamNameAudio, AudioOutput);

    return OutputDataReferences;
}

void FSaturationOperator::Execute()
{
    const float* InputAudio = AudioInput->GetData();
    float* OutputAudio      = AudioOutput->GetData();

    const int32 NumSamples = AudioInput->Num();

    SaturationDSPProcessor.SetSaturationType(*SaturationType); // Set SaturationType first since Gain depends on it
    SaturationDSPProcessor.SetGain(*Gain);
    SaturationDSPProcessor.SetBias(*Bias);
    SaturationDSPProcessor.SetMix(*Mix);
    SaturationDSPProcessor.SetOutLevelDb(*OutLevelDb);

    SaturationDSPProcessor.ProcessAudioBuffer(InputAudio, OutputAudio, NumSamples);
}

const Metasound::FVertexInterface& FSaturationOperator::GetVertexInterface()
{
    static const Metasound::FVertexInterface Interface(
        Metasound::FInputVertexInterface(
            Metasound::TInputDataVertexModel<Metasound::FAudioBuffer>(SaturationNode::InParamNameAudioInput,  LOCTEXT("AudioInputTT", "Audio input.")),
            Metasound::TInputDataVertexModel<float>(SaturationNode::InParamNameGain,                          LOCTEXT("GainTT", "The amount of gain to apply to the sigal. Range = [0.0, 100.0]"), 100.0f),
            Metasound::TInputDataVertexModel<float>(SaturationNode::InParamNameBias,                          LOCTEXT("BiasTT", "The amount of DC bias to apply to the signal. Range = [-1.0, 1.0]"), 0.0f),
            Metasound::TInputDataVertexModel<float>(SaturationNode::InParamNameMix,                           LOCTEXT("MixTT", "The amount of mix between the saturated signal and the input signal. Range = [0.0, 100.0]"), 100.0f),
            Metasound::TInputDataVertexModel<float>(SaturationNode::InParamNameOutLevelDb,                    LOCTEXT("LevelTT", "The amount of gain (in dB) to apply to the signal. Range = [-96dB, +24dB]"), 0.0f),
            Metasound::TInputDataVertexModel<FEnumESaturationType>(SaturationNode::InParamNameSaturationType, LOCTEXT("SaturationTypeTT", "Saturation algorithm to use to process the audio."))
        ),
        Metasound::FOutputVertexInterface(
            Metasound::TOutputDataVertexModel<Metasound::FAudioBuffer>(SaturationNode::OutParamNameAudio, LOCTEXT("AudioOutputTT", "Audio output."))
        )
    );

    return Interface;
}

const Metasound::FNodeClassMetadata& FSaturationOperator::GetNodeInfo()
{
    auto InitNodeInfo = []() -> Metasound::FNodeClassMetadata
    {
        Metasound::FNodeClassMetadata Info;

        Info.ClassName        = { TEXT("UE"), TEXT("Saturation"), TEXT("Audio") };
        Info.MajorVersion     = 1;
        Info.MinorVersion     = 0;
        Info.DisplayName      = LOCTEXT("Metasound_SaturationDisplayName", "Saturation");
        Info.Description      = LOCTEXT("Metasound_SaturationNodeDescription", "Applies saturation to the audio input.");
        Info.Author           = Metasound::PluginAuthor;
        Info.PromptIfMissing  = Metasound::PluginNodeMissingPrompt;
        Info.DefaultInterface = GetVertexInterface();

        return Info;
    };

    static const Metasound::FNodeClassMetadata Info = InitNodeInfo();

    return Info;
}

TUniquePtr<Metasound::IOperator> FSaturationOperator::CreateOperator(const Metasound::FCreateOperatorParams& InParams, Metasound::FBuildErrorArray& OutErrors)
{
    const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
    const Metasound::FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

    Metasound::FAudioBufferReadRef AudioIn  = InputCollection.GetDataReadReferenceOrConstruct<Metasound::FAudioBuffer>(SaturationNode::InParamNameAudioInput, InParams.OperatorSettings);
    FEnumSaturationReadRef InSaturationType = InputCollection.GetDataReadReferenceOrConstruct<FEnumESaturationType>(SaturationNode::InParamNameSaturationType);
    Metasound::FFloatReadRef InGain         = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, SaturationNode::InParamNameGain,       InParams.OperatorSettings);
    Metasound::FFloatReadRef InBias         = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, SaturationNode::InParamNameBias,       InParams.OperatorSettings);
    Metasound::FFloatReadRef InMix          = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, SaturationNode::InParamNameMix,        InParams.OperatorSettings);
    Metasound::FFloatReadRef InOutLevelDb   = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, SaturationNode::InParamNameOutLevelDb, InParams.OperatorSettings);

    return MakeUnique<FSaturationOperator>(InParams.OperatorSettings, AudioIn, InSaturationType, InGain, InBias, InMix, InOutLevelDb);
}


//------------------------------------------------------------------------------------
// FSaturationNode
//------------------------------------------------------------------------------------
FSaturationNode::FSaturationNode(const Metasound::FNodeInitData& InitData)
    : Metasound::FNodeFacade(InitData.InstanceName, InitData.InstanceID, Metasound::TFacadeOperatorClass<FSaturationOperator>())
{
    
}

#undef LOCTEXT_NAMESPACE
