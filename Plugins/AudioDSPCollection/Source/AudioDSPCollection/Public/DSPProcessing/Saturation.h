#pragma once

#include "DSPProcessing/Helpers/ParamSmoother.h"

namespace DSPProcessing
{
    enum class AUDIODSPCOLLECTION_API ESaturationType : int32
    {
        Tape = 0,
        HardClip,
    };

    class AUDIODSPCOLLECTION_API FSaturation
    {
    public:
        FSaturation();
        virtual ~FSaturation();

        void Init();

        void InitGainParam(float SmoothingTimeInMs, float SampleRate);
        void InitBiasParam(float SmoothingTimeInMs, float SampleRate);
        void InitMixParam(float SmoothingTimeInMs, float SampleRate);
        void InitOutLevelParam(float SmoothingTimeInMs, float SampleRate);

        void SetSaturationType(ESaturationType InSaturationType);
        void SetGain(float InGain);
        void SetBias(float InBias);
        void SetMix(float InMixAmount);
        void SetOutLevelDb(float InOutLevelDb);

        void ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, int32 NumSamples);

    private:
        // Saturation graphs https://www.desmos.com/calculator/12d0ysis1g
        FORCEINLINE void ProcessTape(const float* InBuffer, float* OutBuffer, int32 NumSamples);
        FORCEINLINE void ProcessHardClip(const float* InBuffer, float* OutBuffer, int32 NumSamples);

        ESaturationType  SaturationType;
        ParamSmootherLPF GainParamSmoother;
        ParamSmootherLPF BiasParamSmoother;
        ParamSmootherLPF MixParamSmoother;
        ParamSmootherLPF OutLevelParamSmoother;

        // Function pointer that points to the selected saturation type
        void (FSaturation::*SelectedSaturationTypePtr)(const float* /*InBuffer*/, float* /*OutBuffer*/, int32 /*NumSamples*/);
    };
}
