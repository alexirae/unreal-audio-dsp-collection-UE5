#include "MetasoundNodes/MetasoundGainNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_GainNode"


METASOUND_REGISTER_NODE(FGainNode)


//------------------------------------------------------------------------------------
// FGainOperator
//------------------------------------------------------------------------------------
FGainOperator::FGainOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudioInput, const Metasound::FFloatReadRef& InGain)
    : AudioInput(InAudioInput)
    , AudioOutput(Metasound::FAudioBufferWriteRef::CreateNew(InSettings))
    , Gain(InGain)
{
    GainDSPProcessor.Init();

    const float SampleRate            = InSettings.GetSampleRate();
    constexpr float SmoothingTimeInMs = 21.33f;

    GainDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
}

Metasound::FDataReferenceCollection FGainOperator::GetInputs() const
{
    Metasound::FDataReferenceCollection InputDataReferences;

    InputDataReferences.AddDataReadReference(GainNode::InParamNameAudioInput, AudioInput);
    InputDataReferences.AddDataReadReference(GainNode::InParamNameGain, Gain);

    return InputDataReferences;
}

Metasound::FDataReferenceCollection FGainOperator::GetOutputs() const
{
    Metasound::FDataReferenceCollection OutputDataReferences;

    OutputDataReferences.AddDataReadReference(GainNode::OutParamNameAudio, AudioOutput);

    return OutputDataReferences;
}

void FGainOperator::Execute()
{
    const float* InputAudio = AudioInput->GetData();
    float* OutputAudio      = AudioOutput->GetData();

    const int32 NumSamples = AudioInput->Num();

    GainDSPProcessor.SetGain(*Gain);

    GainDSPProcessor.ProcessAudioBuffer(InputAudio, OutputAudio, NumSamples);
}

const Metasound::FVertexInterface& FGainOperator::GetVertexInterface()
{
    static const Metasound::FVertexInterface Interface(
        Metasound::FInputVertexInterface(
            Metasound::TInputDataVertexModel<Metasound::FAudioBuffer>(GainNode::InParamNameAudioInput, LOCTEXT("AudioInputTT", "Audio input.")),
            Metasound::TInputDataVertexModel<float>(GainNode::InParamNameGain, LOCTEXT("GainTT", "The amount of Gain to apply to the signal."), 1.0f)
        ),
        Metasound::FOutputVertexInterface(
            Metasound::TOutputDataVertexModel<Metasound::FAudioBuffer>(GainNode::OutParamNameAudio, LOCTEXT("AudioOutputTT", "Audio output."))
        )
    );

    return Interface;
}

const Metasound::FNodeClassMetadata& FGainOperator::GetNodeInfo()
{
    auto InitNodeInfo = []() -> Metasound::FNodeClassMetadata
    {
        Metasound::FNodeClassMetadata Info;

        Info.ClassName        = { TEXT("UE"), TEXT("Gain"), TEXT("Audio") };
        Info.MajorVersion     = 1;
        Info.MinorVersion     = 0;
        Info.DisplayName      = LOCTEXT("Metasound_GainDisplayName", "Gain");
        Info.Description      = LOCTEXT("Metasound_GainNodeDescription", "Applies gain to the audio input.");
        Info.Author           = Metasound::PluginAuthor;
        Info.PromptIfMissing  = Metasound::PluginNodeMissingPrompt;
        Info.DefaultInterface = GetVertexInterface();

        return Info;
    };

    static const Metasound::FNodeClassMetadata Info = InitNodeInfo();

    return Info;
}

TUniquePtr<Metasound::IOperator> FGainOperator::CreateOperator(const Metasound::FCreateOperatorParams& InParams, Metasound::FBuildErrorArray& OutErrors)
{
    const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
    const Metasound::FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

    Metasound::FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<Metasound::FAudioBuffer>(GainNode::InParamNameAudioInput, InParams.OperatorSettings);
    Metasound::FFloatReadRef InGain        = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, GainNode::InParamNameGain, InParams.OperatorSettings);

    return MakeUnique<FGainOperator>(InParams.OperatorSettings, AudioIn, InGain);
}


//------------------------------------------------------------------------------------
// FGainNode
//------------------------------------------------------------------------------------
FGainNode::FGainNode(const Metasound::FNodeInitData& InitData)
    : Metasound::FNodeFacade(InitData.InstanceName, InitData.InstanceID, Metasound::TFacadeOperatorClass<FGainOperator>())
{
    
}

#undef LOCTEXT_NAMESPACE
