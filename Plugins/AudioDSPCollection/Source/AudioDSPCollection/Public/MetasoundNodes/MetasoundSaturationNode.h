#include "DSPProcessing/Saturation.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

namespace Metasound
{
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_SaturationNode"

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

    DECLARE_METASOUND_ENUM(DSPProcessing::ESaturationType, DSPProcessing::ESaturationType::Tape, AUDIODSPCOLLECTION_API, FEnumESaturationType, FEnumSaturationTypeInfo, FEnumSaturationReadRef, FEnumSaturationWriteRef);

    DEFINE_METASOUND_ENUM_BEGIN(DSPProcessing::ESaturationType, FEnumESaturationType,  "SaturationType")
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

#undef LOCTEXT_NAMESPACE


    //------------------------------------------------------------------------------------
    // FSaturationOperator
    //------------------------------------------------------------------------------------
    class FSaturationOperator : public TExecutableOperator<FSaturationOperator>
    {
    public:
        static const FNodeClassMetadata& GetNodeInfo();
        static const FVertexInterface& GetVertexInterface();
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);

        FSaturationOperator(const FOperatorSettings& InSettings,
                            const FAudioBufferReadRef& InAudioInput,
                            const FFloatReadRef& InGain,
                            const FFloatReadRef& InBias,
                            const FFloatReadRef& InMix,
                            const FFloatReadRef& InOutLevelDb,
                            const FEnumSaturationReadRef& InSaturationType);

        virtual FDataReferenceCollection GetInputs()  const override;
        virtual FDataReferenceCollection GetOutputs() const override;

        void Execute();

    private:
        DSPProcessing::FSaturation SaturationDSPProcessor;

        FAudioBufferReadRef  AudioInput;
        FAudioBufferWriteRef AudioOutput;

        FFloatReadRef          Gain;
        FFloatReadRef          Bias;
        FFloatReadRef          Mix;
        FFloatReadRef          OutLevelDb;
        FEnumSaturationReadRef SaturationType;
    };


    //------------------------------------------------------------------------------------
    // FSaturationNode
    //------------------------------------------------------------------------------------
    class FSaturationNode : public FNodeFacade
    {
    public:
        // Constructor used by the Metasound Frontend.
        FSaturationNode(const FNodeInitData& InitData);
    };
}
