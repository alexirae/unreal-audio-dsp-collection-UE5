#include "DSPProcessing/Saturation.h"
#include "CoreMinimal.h"


namespace DSPProcessing
{
    FSaturation::FSaturation()
        : SaturationType(ESaturationType::Tape)
        , SelectedSaturationTypePtr(&FSaturation::Tape)
    {
        
    }

    FSaturation::~FSaturation()
    {
        
    }

    void FSaturation::Init()
    {
        
    }

    void FSaturation::InitGainParam(float SmoothingTimeInMs, float SampleRate)
    {
        GainParamSmoother.Init(SmoothingTimeInMs, SampleRate);
    }

    void FSaturation::InitBiasParam(float SmoothingTimeInMs, float SampleRate)
    {
        BiasParamSmoother.Init(SmoothingTimeInMs, SampleRate);
    }

    void FSaturation::InitMixParam(float SmoothingTimeInMs, float SampleRate)
    {
        MixParamSmoother.Init(SmoothingTimeInMs, SampleRate);
    }

    void FSaturation::InitOutLevelParam(float SmoothingTimeInMs, float SampleRate)
    {
        OutLevelParamSmoother.Init(SmoothingTimeInMs, SampleRate);
    }

    void FSaturation::SetSaturationType(ESaturationType InSaturationType)
    {
        SaturationType = InSaturationType;

        switch (SaturationType)
        {
            default:
            case ESaturationType::Tape:
                SelectedSaturationTypePtr = &FSaturation::Tape;
                break;
            case ESaturationType::Tape2:
                SelectedSaturationTypePtr = &FSaturation::Tape2;
                break;
            case ESaturationType::Overdrive:
                SelectedSaturationTypePtr = &FSaturation::Overdrive;
                break;
            case ESaturationType::Tube:
                SelectedSaturationTypePtr = &FSaturation::Tube;
                break;
            case ESaturationType::Tube2:
                SelectedSaturationTypePtr = &FSaturation::Tube2;
                break;
            case ESaturationType::Distortion:
                SelectedSaturationTypePtr = &FSaturation::Distortion;
                break;
            case ESaturationType::Metal:
                SelectedSaturationTypePtr = &FSaturation::Metal;
                break;
            case ESaturationType::Fuzz:
                SelectedSaturationTypePtr = &FSaturation::Fuzz;
                break;
            case ESaturationType::HardClip:
                SelectedSaturationTypePtr = &FSaturation::HardClip;
                break;
            case ESaturationType::Foldback:
                SelectedSaturationTypePtr = &FSaturation::Foldback;
                break;
            case ESaturationType::HalfWaveRectifier:
                SelectedSaturationTypePtr = &FSaturation::HalfWaveRectifier;
                break;
            case ESaturationType::FullWaveRectifier:
                SelectedSaturationTypePtr = &FSaturation::FullWaveRectifier;
                break;
        }
    }

    void FSaturation::SetGain(float InGain)
    {
        InGain = FMath::Clamp(InGain, 0.0f, 100.0f) * 0.01f; // Clamp and Normalize

        switch (SaturationType)
        {
            default:
            case ESaturationType::Tape:
                GainParamSmoother.SetNewParamValue(1.0f + 19.0f * InGain);  // Range = [1, 20]
                break;
            case ESaturationType::Tape2:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::Overdrive:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::Tube:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::Tube2:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::Distortion:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::Metal:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::Fuzz:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::HardClip:
                GainParamSmoother.SetNewParamValue(1.0f + 300.0f * InGain); // Range = [1, 300]
                break;
            case ESaturationType::Foldback:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::HalfWaveRectifier:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
            case ESaturationType::FullWaveRectifier:
                GainParamSmoother.SetNewParamValue(1.0f);
                break;
        }
    }

    void FSaturation::SetBias(float InBias)
    {
        InBias = FMath::Clamp(InBias, -1.0f, 1.0f);
        BiasParamSmoother.SetNewParamValue(InBias);
    }

    void FSaturation::SetMix(float InMixAmount)
    {
        InMixAmount = FMath::Clamp(InMixAmount, 0.0f, 100.0f) * 0.01f; // Clamp and Normalize
        MixParamSmoother.SetNewParamValue(InMixAmount);
    }

    void FSaturation::SetOutLevelDb(float InOutLevelDb)
    {
        InOutLevelDb = FMath::Clamp(InOutLevelDb, -96.0f, 24.0f);
        const float InOutLevelLinear = (InOutLevelDb == -96.0f) ? 0.0f : Audio::ConvertToLinear(InOutLevelDb);

        OutLevelParamSmoother.SetNewParamValue(InOutLevelLinear);
    }

    void FSaturation::ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        //TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FSaturation::ProcessAudioBuffer"))
        
        // Skip processing if OutLevel == 0
        if (OutLevelParamSmoother.GetValue() == 0.0f)
        {
            FMemory::Memzero(OutBuffer, sizeof(float) * NumSamples);
            return;
        }

        // Skip processing if OutLevel==1 and Mix == 0
        if (OutLevelParamSmoother.GetValue() == 1.0f && MixParamSmoother.GetValue() == 0.0f)
        {
            FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
            return;
        }

        // Process with selected saturation algorithm
        (this->*(SelectedSaturationTypePtr))(InBuffer, OutBuffer, NumSamples);
    }

    void FSaturation::Tape(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.SmoothedParamValue();
        //    const float Bias     = BiasParamSmoother.SmoothedParamValue();
        //    const float OutLevel = OutLevelParamSmoother.SmoothedParamValue();
        //    const float Mix      = MixParamSmoother.SmoothedParamValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;
        //
        //    float Out = Gain_x_In / FMath::Sqrt(Gain_x_In * Gain_x_In + 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        const VectorRegister& VOnes = GlobalVectorConstants::FloatOne;

        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = Gain_x_In / FMath::Sqrt(Gain_x_In * Gain_x_In + 1.0f);
            VectorRegister Out = VectorMultiplyAdd(Gain_x_In, Gain_x_In, VOnes);
            Out = VectorReciprocalSqrt(Out);
            Out = VectorMultiply(Gain_x_In, Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister One_Minus_Mix      = VectorSubtract(VOnes, VMix);
            const VectorRegister One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Tape2(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::Overdrive(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::Tube(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::Tube2(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::Distortion(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::Metal(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::Fuzz(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::HardClip(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.SmoothedParamValue();
        //    const float Bias     = BiasParamSmoother.SmoothedParamValue();
        //    const float OutLevel = OutLevelParamSmoother.SmoothedParamValue();
        //    const float Mix      = MixParamSmoother.SmoothedParamValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;
        //
        //    float Out = FMath::Clamp(Gain_x_In, -1.0f, 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        const VectorRegister& VOnes = GlobalVectorConstants::FloatOne;
        const VectorRegister& VMinusOnes = GlobalVectorConstants::FloatMinusOne;

        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain = GainParamSmoother.GetValue();
            const float CurrentBias = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix = MixParamSmoother.GetValue();

            const VectorRegister VGain = VectorLoadFloat1(&CurrentGain);
            const VectorRegister VBias = VectorLoadFloat1(&CurrentBias);
            const VectorRegister VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister VMix = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = FMath::Clamp(Gain_x_In, -1.0f, 1.0f);
            VectorRegister Out = VectorMax(VectorMin(Gain_x_In, VOnes), VMinusOnes);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister One_Minus_Mix = VectorSubtract(VOnes, VMix);
            const VectorRegister One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Foldback(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::HalfWaveRectifier(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }

    void FSaturation::FullWaveRectifier(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * NumSamples);
    }
}
