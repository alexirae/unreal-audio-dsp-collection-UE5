#include "MetasoundNodes/MetasoundSaturationNode.h"

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SaturationNode"

namespace Metasound
{
    //------------------------------------------------------------------------------------
    // FSaturationOperator
    //------------------------------------------------------------------------------------
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

    FDataReferenceCollection FSaturationOperator::GetInputs() const
    {
        using namespace SaturationNode;

        FDataReferenceCollection InputDataReferences;

        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameAudioInput), AudioInput);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameGain), Gain);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameBias), Bias);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameMix), Mix);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameOutLevelDb), OutLevelDb);
        InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InParamNameSaturationType), SaturationType);

        return InputDataReferences;
    }

    FDataReferenceCollection FSaturationOperator::GetOutputs() const
    {
        using namespace SaturationNode;

        FDataReferenceCollection OutputDataReferences;

        OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutParamNameAudio), AudioOutput);

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

    const FNodeClassMetadata& FSaturationOperator::GetNodeInfo()
    {
        auto InitNodeInfo = []() -> FNodeClassMetadata
        {
            FNodeClassMetadata Info;

            Info.ClassName         = { TEXT("UE"), TEXT("Saturation"), TEXT("Audio") };
            Info.MajorVersion      = 1;
            Info.MinorVersion      = 1;
            Info.DisplayName       = LOCTEXT("Metasound_SaturationDisplayName", "Saturation");
            Info.Description       = LOCTEXT("Metasound_SaturationNodeDescription", "Applies saturation to the audio input.");
            Info.Author            = PluginAuthor;
            Info.PromptIfMissing   = PluginNodeMissingPrompt;
            Info.DefaultInterface  = GetVertexInterface();
            Info.CategoryHierarchy = { LOCTEXT("Metasound_SaturationNodeCategory", "Saturation") };

            return Info;
        };

        static const FNodeClassMetadata Info = InitNodeInfo();

        return Info;
    }

    TUniquePtr<IOperator> FSaturationOperator::CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
    {
        using namespace SaturationNode;

        const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
        const FInputVertexInterface& InputInterface     = GetVertexInterface().GetInputInterface();

        FAudioBufferReadRef AudioIn             = InputCollection.GetDataReadReferenceOrConstruct<FAudioBuffer>(METASOUND_GET_PARAM_NAME(InParamNameAudioInput),                           InParams.OperatorSettings);
        FFloatReadRef InGain                    = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameGain),       InParams.OperatorSettings);
        FFloatReadRef InBias                    = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameBias),       InParams.OperatorSettings);
        FFloatReadRef InMix                     = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameMix),        InParams.OperatorSettings);
        FFloatReadRef InOutLevelDb              = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, METASOUND_GET_PARAM_NAME(InParamNameOutLevelDb), InParams.OperatorSettings);
        FEnumSaturationReadRef InSaturationType = InputCollection.GetDataReadReferenceOrConstruct<FEnumESaturationType>(METASOUND_GET_PARAM_NAME(InParamNameSaturationType));

        return MakeUnique<FSaturationOperator>(InParams.OperatorSettings, AudioIn, InGain, InBias, InMix, InOutLevelDb, InSaturationType);
    }


    //------------------------------------------------------------------------------------
    // FSaturationNode
    //------------------------------------------------------------------------------------
    FSaturationNode::FSaturationNode(const FNodeInitData& InitData)
        : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSaturationOperator>())
    {
    
    }

    METASOUND_REGISTER_NODE(FSaturationNode)
}

#undef LOCTEXT_NAMESPACE
