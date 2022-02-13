#include "DSPProcessing/Gain.h"
#include "MetasoundEnumRegistrationMacro.h"


namespace GainNode
{
    static const TCHAR* InParamNameAudioInput = TEXT("In");
    static const TCHAR* InParamNameGain       = TEXT("Gain");
    static const TCHAR* OutParamNameAudio     = TEXT("Out");
}

//////////////////////////////////////////////////////////////////////////////////////

class FGainOperator : public Metasound::TExecutableOperator<FGainOperator>
{
public:
    static const Metasound::FNodeClassMetadata& GetNodeInfo();
    static const Metasound::FVertexInterface& GetVertexInterface();
    static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FCreateOperatorParams& InParams, Metasound::FBuildErrorArray& OutErrors);

    FGainOperator(const Metasound::FOperatorSettings& InSettings, const Metasound::FAudioBufferReadRef& InAudioInput, const Metasound::FFloatReadRef& InGain);

    virtual Metasound::FDataReferenceCollection GetInputs()  const override;
    virtual Metasound::FDataReferenceCollection GetOutputs() const override;

    void Execute();

private:
    Metasound::FAudioBufferReadRef  AudioInput;
    Metasound::FAudioBufferWriteRef AudioOutput;

    DSPProcessing::FGain GainDSPProcessor;

    Metasound::FFloatReadRef Gain;
};

//////////////////////////////////////////////////////////////////////////////////////

class FGainNode : public Metasound::FNodeFacade
{
public:
    // Constructor used by the Metasound Frontend.
    FGainNode(const Metasound::FNodeInitData& InitData);
};

//////////////////////////////////////////////////////////////////////////////////////
