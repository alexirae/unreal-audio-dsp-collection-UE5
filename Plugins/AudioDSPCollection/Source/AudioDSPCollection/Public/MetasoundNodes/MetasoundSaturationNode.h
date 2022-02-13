#include "DSPProcessing/Saturation.h"
#include "MetasoundEnumRegistrationMacro.h"


namespace SaturationNode
{
    static const TCHAR* InParamNameAudioInput     = TEXT("In");
    static const TCHAR* InParamNameGain           = TEXT("Gain");
    static const TCHAR* InParamNameBias           = TEXT("Bias");
    static const TCHAR* InParamNameMix            = TEXT("Mix");
    static const TCHAR* InParamNameOutLevelDb     = TEXT("Out Level (dB)");
    static const TCHAR* InParamNameSaturationType = TEXT("Saturation Type");
    static const TCHAR* OutParamNameAudio         = TEXT("Out");
}

DECLARE_METASOUND_ENUM(DSPProcessing::ESaturationType, DSPProcessing::ESaturationType::Tape, AUDIODSPCOLLECTION_API, FEnumESaturationType, FEnumSaturationTypeInfo, FEnumSaturationReadRef, FEnumSaturationWriteRef);

#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SaturationNode"
    DEFINE_METASOUND_ENUM_BEGIN(DSPProcessing::ESaturationType, FEnumESaturationType, "SaturationType")
        DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::Tape,     LOCTEXT("TapeDescription", "Tape"),         LOCTEXT("TapeTT", "Tape Saturation")),
        DEFINE_METASOUND_ENUM_ENTRY(DSPProcessing::ESaturationType::HardClip, LOCTEXT("HardClipDescription", "HardClip"), LOCTEXT("HardClipTT", "Hard Clip Saturation")),
    DEFINE_METASOUND_ENUM_END()
#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////

class FSaturationOperator : public Metasound::TExecutableOperator<FSaturationOperator>
{
public:
    static const Metasound::FNodeClassMetadata& GetNodeInfo();
    static const Metasound::FVertexInterface& GetVertexInterface();
    static TUniquePtr<Metasound::IOperator> CreateOperator(const Metasound::FCreateOperatorParams& InParams, Metasound::FBuildErrorArray& OutErrors);

    FSaturationOperator(const Metasound::FOperatorSettings& InSettings,
                        const Metasound::FAudioBufferReadRef& InAudioInput,
                        const FEnumSaturationReadRef& InSaturationType,
                        const Metasound::FFloatReadRef& InGain,
                        const Metasound::FFloatReadRef& InBias,
                        const Metasound::FFloatReadRef& InMix,
                        const Metasound::FFloatReadRef& InOutLevelDb);

    virtual Metasound::FDataReferenceCollection GetInputs()  const override;
    virtual Metasound::FDataReferenceCollection GetOutputs() const override;

    void Execute();

private:
    Metasound::FAudioBufferReadRef  AudioInput;
    Metasound::FAudioBufferWriteRef AudioOutput;

    DSPProcessing::FSaturation SaturationDSPProcessor;

    FEnumSaturationReadRef   SaturationType;
    Metasound::FFloatReadRef Gain;
    Metasound::FFloatReadRef Bias;
    Metasound::FFloatReadRef Mix;
    Metasound::FFloatReadRef OutLevelDb;
};

//////////////////////////////////////////////////////////////////////////////////////

class FSaturationNode : public Metasound::FNodeFacade
{
public:
    // Constructor used by the Metasound Frontend.
    FSaturationNode(const Metasound::FNodeInitData& InitData);
};

//////////////////////////////////////////////////////////////////////////////////////