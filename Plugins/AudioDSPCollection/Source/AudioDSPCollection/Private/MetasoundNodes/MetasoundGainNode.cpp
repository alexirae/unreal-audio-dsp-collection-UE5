#include "MetasoundNodes/MetasoundGainNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_GainNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FGainOperator
    //------------------------------------------------------------------------------------
    FGainOperator::FGainOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InGain)
        : AudioInput(InAudioInput)
        , AudioOutput(FAudioBufferWriteRef::CreateNew(InSettings))
        , Gain(InGain)
    {
        GainDSPProcessor.Init();

        const float SampleRate            = InSettings.GetSampleRate();
        constexpr float SmoothingTimeInMs = 21.33f;

        GainDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
    }

    FDataReferenceCollection FGainOperator::GetInputs() const
    {
        using namespace GainNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameGain), Gain);

        return InputDataReferences;
    }

    FDataReferenceCollection FGainOperator::GetOutputs() const
    {
        using namespace GainNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

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

    const FNodeClassMetadata& FGainOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName        = { TEXT("UE"), TEXT("Gain"), TEXT("Audio") };
            Info.MajorVersion     = 1;
            Info.MinorVersion     = 1;
            Info.DisplayName      = LOCTEXT("Metasound_GainDisplayName", "Gain");
            Info.Description      = LOCTEXT("Metasound_GainNodeDescription", "Applies gain to the audio input.");
            Info.Author           = PluginAuthor;
            Info.PromptIfMissing  = PluginNodeMissingPrompt;
            Info.DefaultInterface = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_GainNodeCategory", "Utils") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FGainOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace GainNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FAudioBufferReadRef AudioIn = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput),                     InParams.OperatorSettings);
        FFloatReadRef InGain        = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameGain), InParams.OperatorSettings);

        return MakeUnique<FGainOperator>(InParams.OperatorSettings, AudioIn, InGain);
    }


    //------------------------------------------------------------------------------------
    // FGainNode
    //------------------------------------------------------------------------------------
    FGainNode::FGainNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FGainOperator>())
    {

    }

    METASOUND_REGISTER_NODE(FGainNode)
}

#undef LOCTEXT_NAMESPACE
