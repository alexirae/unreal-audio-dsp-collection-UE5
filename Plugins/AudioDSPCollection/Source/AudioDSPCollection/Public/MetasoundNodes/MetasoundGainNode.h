#include "DSPProcessing/Gain.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_GainNode"

    namespace GainNode
    {
        METASOUND_PARAM(InParamNameAudioInput, "In",   "Audio input.")
        METASOUND_PARAM(InParamNameGain,       "Gain", "The amount of gain to apply to the input signal. Range = [0.0, 1.0]")
        METASOUND_PARAM(OutParamNameAudio,     "Out",  "Audio output.")
    }

#undef LOCTEXT_NAMESPACE

    //------------------------------------------------------------------------------------
    // FGainOperator
    //------------------------------------------------------------------------------------
    class FGainOperator : public TExecutableOperator<FGainOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FGainOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InGain);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        FAudioBufferReadRef  AudioInput;
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
        FGainNode(const FNodeInitData& InitData);
    };
}
