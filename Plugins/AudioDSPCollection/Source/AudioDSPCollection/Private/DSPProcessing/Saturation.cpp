#include "DSPProcessing/Saturation.h"
#include "CoreMinimal.h"


namespace DSPProcessing
{
    namespace FSaturationUtils
    {
        const VectorRegister4Float& VMinusOnes = GlobalVectorConstants::FloatMinusOne;
        const VectorRegister4Float& VZeros     = GlobalVectorConstants::FloatZero;
        const VectorRegister4Float& VOneHalf   = GlobalVectorConstants::FloatOneHalf;
        const VectorRegister4Float& VOnes      = GlobalVectorConstants::FloatOne;
        const VectorRegister4Float& VTwos      = GlobalVectorConstants::FloatTwo;

        constexpr VectorRegister4Float VThrees = MakeVectorRegisterFloatConstant(3.0f, 3.0f, 3.0f, 3.0f);


        FORCEINLINE float MapFromNormalizedRange(float value, float minRange, float maxRange)
        {
            return minRange + value * (maxRange - minRange);
        }

        FORCEINLINE VectorRegister4Float VectorClampMinusOneToOne(const VectorRegister4Float& Vec)
        {
            return VectorMax(VectorMin(Vec, FSaturationUtils::VOnes), FSaturationUtils::VMinusOnes);
        }

        FORCEINLINE float FastTanh(float x)
        {
            const float AbsX = FMath::Abs(x);
            const float XSqr = x * x;

            const float z = x * (1.0f + AbsX +
                                  (1.05622909486427f + 0.215166815390934f * XSqr * AbsX) * XSqr);

            return z / (1.02718982441289f + FMath::Abs(z));
        }

        FORCEINLINE VectorRegister4Float VectorTanh(const VectorRegister4Float& X)
        {
        #if UE_PLATFORM_MATH_USE_SVML
	        return _mm_tanh_ps(X);
        #else
	        AlignedFloat4 Floats(X);
	        Floats[0] = FastTanh(Floats[0]);
	        Floats[1] = FastTanh(Floats[1]);
	        Floats[2] = FastTanh(Floats[2]);
	        Floats[3] = FastTanh(Floats[3]);
	        return Floats.ToVectorRegister();
        #endif
        }
    }

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
        InGain = FMath::Clamp(InGain, 0.0f, 100.0f) * 0.01f; // Clamp and Normalize [0, 1]

        switch (SaturationType)
        {
            default:
            case ESaturationType::Tape:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 1.0f, 20.0f);
                GainParamSmoother.SetNewParamValue(InGain);  
                break;
            case ESaturationType::Tape2:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 0.000001f, 35.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Overdrive:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 1.0f, 20.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Tube:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 1.0f, 40.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Tube2:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 1.0f, 45.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Distortion:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 0.000001f, 80.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Metal:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 1.0f, 100.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Fuzz:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 0.5f, 35.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::HardClip:
                InGain = FSaturationUtils::MapFromNormalizedRange(InGain, 1.0f, 300.0f);
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::Foldback:
                GainParamSmoother.SetNewParamValue(InGain);
                break;
            case ESaturationType::HalfWaveRectifier:
                break;
            case ESaturationType::FullWaveRectifier:
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
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
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
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = Gain_x_In / FMath::Sqrt(Gain_x_In * Gain_x_In + 1.0f);
            VectorRegister4Float Out = VectorMultiplyAdd(Gain_x_In, Gain_x_In, FSaturationUtils::VOnes);
            Out = VectorReciprocalSqrt(Out);
            Out = VectorMultiply(Gain_x_In, Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Tape2(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;
        //
        //    float Out = FMath::Atan(Gain_x_In) / FMath::Atan(Gain);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = FMath::Atan(Gain_x_In) / FMath::Atan(Gain);
            const VectorRegister4Float OneOverAtanGain = VectorReciprocal(VectorATan(VGain));
            VectorRegister4Float Out = VectorATan(Gain_x_In);
            Out = VectorMultiply(Out, OneOverAtanGain);

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Overdrive(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;
        //
        //    const float Gain_x_ClampIn       = Gain * FMath::Clamp(In_Plus_Bias, -1.0f, 1.0f);
        //    const float Clamp_Gain_x_ClampIn = FMath::Clamp(Gain_x_ClampIn, -1.0f, 1.0f);

        //    float Out = 0.5f * FMath::Clamp(Gain_x_In, -1.0f, 1.0f) * (3.0f - Clamp_Gain_x_ClampIn * Clamp_Gain_x_ClampIn);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain = GainParamSmoother.GetValue();
            const float CurrentBias = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //const float Gain_x_ClampIn = Gain * FMath::Clamp(In_Plus_Bias, -1.0f, 1.0f);
            const VectorRegister4Float ClampIn        = FSaturationUtils::VectorClampMinusOneToOne(In_Plus_Bias);
            const VectorRegister4Float Gain_x_ClampIn = VectorMultiply(VGain, ClampIn);

            //const float Clamp_Gain_x_ClampIn = FMath::Clamp(Gain_x_ClampIn, -1.0f, 1.0f);
            const VectorRegister4Float Clamp_Gain_x_ClampIn = FSaturationUtils::VectorClampMinusOneToOne(Gain_x_ClampIn);

            //float Out = 0.5f * FMath::Clamp(Gain_x_In, -1.0f, 1.0f) * (3.0f - Clamp_Gain_x_ClampIn * Clamp_Gain_x_ClampIn);
            const VectorRegister4Float Clamp_Gain_x_In                      = FSaturationUtils::VectorClampMinusOneToOne(Gain_x_In);
            const VectorRegister4Float Clamp_Gain_x_ClampIn_Sqr             = VectorMultiply(Clamp_Gain_x_ClampIn, Clamp_Gain_x_ClampIn);
            const VectorRegister4Float Three_Minus_Clamp_Gain_x_ClampIn_Sqr = VectorSubtract(FSaturationUtils::VThrees, Clamp_Gain_x_ClampIn_Sqr);

            VectorRegister4Float Out = VectorMultiply(FSaturationUtils::VOneHalf, VectorMultiply(Clamp_Gain_x_In, Three_Minus_Clamp_Gain_x_ClampIn_Sqr));

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Tube(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;
        //
        //    float Out = (In_Plus_Bias > 0.0f) ? Gain_x_In : Gain_x_In * FMath::InvSqrt(Gain_x_In * Gain_x_In + 1.0f);
        //    Out = FMath::Clamp(Out, -1.0f, 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = (In_Plus_Bias > 0.0f) ? Gain_x_In : Gain_x_In * FMath::InvSqrt(Gain_x_In * Gain_x_In + 1.0f);
            const VectorRegister4Float Gain_x_In_Sqrt_Plus_One = VectorMultiplyAdd(Gain_x_In, Gain_x_In, FSaturationUtils::VOnes);
            VectorRegister4Float Out = (VectorAnyGreaterThan(In_Plus_Bias, FSaturationUtils::VZeros) > 0) ? Gain_x_In : VectorMultiply(Gain_x_In, VectorReciprocalSqrt(Gain_x_In_Sqrt_Plus_One));
            
            //Out = FMath::Clamp(Gain_x_In, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Gain_x_In);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Tube2(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;

        //    float Out = FMath::Pow(FMath::Clamp(In_Plus_Bias, -1.0f, 1.0f) + 1.0f, Gain) - 1.0f;
        //
        //    Out = FMath::Clamp(Out, -1.0f, 1.0f);

        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = FMath::Pow(FMath::Clamp(In_Plus_Bias, -1.0f, 1.0f) + 1.0f, Gain) - 1.0f;
            const VectorRegister4Float OneOverAtanGain = VectorReciprocal(FSaturationUtils::VectorTanh(VGain));
            const VectorRegister4Float Clamp_In_Plus_Bias_Plus_One = VectorAdd(FSaturationUtils::VectorClampMinusOneToOne(In_Plus_Bias), FSaturationUtils::VOnes);
            const VectorRegister4Float Pow_Clamp_In_Plus_Bias_Plus_One_To_Gain = VectorPow(Clamp_In_Plus_Bias_Plus_One, VGain);
            VectorRegister4Float Out = VectorSubtract(Pow_Clamp_In_Plus_Bias_Plus_One_To_Gain, FSaturationUtils::VOnes);

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Distortion(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;
        //
        //    float Out = FSaturationUtils::FastTanh(Gain_x_In) / FSaturationUtils::FastTanh(Gain);
        //
        //    Out = FMath::Clamp(Out, -1.0f, 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = FMath::Atan(Gain_x_In) / FMath::Atan(Gain);
            const VectorRegister4Float OneOverAtanGain = VectorReciprocal(FSaturationUtils::VectorTanh(VGain));
            VectorRegister4Float Out = FSaturationUtils::VectorTanh(Gain_x_In);
            Out = VectorMultiply(Out, OneOverAtanGain);

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Metal(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Gain_x_In    = Gain * In_Plus_Bias;

        //    const float Abs_Clamp_Gain_x_In           = FMath::Abs(FMath::Clamp(Gain_x_In, -1.0f, 1.0f));
        //    const float Two_Minus_Abs_Clamp_Gain_x_In = 2.0f - Abs_Clamp_Gain_x_In;

        //    const float Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In = Abs_Clamp_Gain_x_In * Two_Minus_Abs_Clamp_Gain_x_In;

        //    float Out = (In_Plus_Bias >= 0.0f) ? Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In : -Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In;
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //const float Abs_Clamp_Gain_x_In = FMath::Abs(FMath::Clamp(Gain_x_In, -1.0f, 1.0f));
            const VectorRegister4Float Abs_Clamp_Gain_x_In = VectorAbs(FSaturationUtils::VectorClampMinusOneToOne(In_Plus_Bias));

            //const float Two_Minus_Abs_Clamp_Gain_x_In = 2.0f - Abs_Clamp_Gain_x_In;
            const VectorRegister4Float Two_Minus_Abs_Clamp_Gain_x_In = VectorSubtract(FSaturationUtils::VTwos, Abs_Clamp_Gain_x_In);

            //const float Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In = Abs_Clamp_Gain_x_In * Two_Minus_Abs_Clamp_Gain_x_In;
            const VectorRegister4Float Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In = VectorMultiply(Abs_Clamp_Gain_x_In, Two_Minus_Abs_Clamp_Gain_x_In);

            //float Out = (In_Plus_Bias >= 0.0f) ? Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In : -Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In;
            VectorRegister4Float Out = (VectorAnyGreaterThan(In_Plus_Bias, FSaturationUtils::VZeros) > 0) ? Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In 
                                                                                                          : VectorNegate(Abs_Clamp_Gain_x_In_x_Two_Minus_Abs_Clamp_Gain_x_In);
            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Fuzz(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        for (int32 i = 0; i < NumSamples; ++i)
        {
            const float Gain     = GainParamSmoother.GetValue();
            const float Bias     = BiasParamSmoother.GetValue();
            const float OutLevel = OutLevelParamSmoother.GetValue();
            const float Mix      = MixParamSmoother.GetValue();
        
            const float In = InBuffer[i];
        
            const float In_Plus_Bias = In + Bias;
            const float Gain_x_In    = Gain * In_Plus_Bias;

            const float Gain_x_In_Over_Abs_Gain_x_In = Gain_x_In / FMath::Abs(Gain_x_In);

            float Out = -Gain_x_In_Over_Abs_Gain_x_In * (1.0f - FMath::Exp(Gain_x_In * Gain_x_In_Over_Abs_Gain_x_In));
        
            Out = FMath::Clamp(Out, -1.0f, 1.0f);
        
            Out = Out * Mix + (1.0f - Mix) * In;
            Out = Out * OutLevel;
        
            OutBuffer[i] = Out;
        }

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //const float Gain_x_In_Over_Abs_Gain_x_In = Gain_x_In / FMath::Abs(Gain_x_In);
            const VectorRegister4Float Gain_x_In_Over_Abs_Gain_x_In = VectorMultiply(Gain_x_In, VectorReciprocal(VectorAbs(Gain_x_In)));

            //float Out = -Gain_x_In_Over_Abs_Gain_x_In * (1.0f - FMath::Exp(Gain_x_In * Gain_x_In_Over_Abs_Gain_x_In));
            const VectorRegister4Float Gain_x_Gain_x_In_Over_Abs_Gain_x_In = VectorMultiply(Gain_x_In, Gain_x_In_Over_Abs_Gain_x_In);
            const VectorRegister4Float Exp_Gain_x_Gain_x_In_Over_Abs_Gain_x_In = VectorExp(Gain_x_Gain_x_In_Over_Abs_Gain_x_In);
            const VectorRegister4Float One_Minus_Exp_Gain_x_Gain_x_In_Over_Abs_Gain_x_In = VectorSubtract(FSaturationUtils::VOnes, Exp_Gain_x_Gain_x_In_Over_Abs_Gain_x_In);
            VectorRegister4Float Out = VectorMultiply(VectorNegate(Gain_x_In_Over_Abs_Gain_x_In), One_Minus_Exp_Gain_x_Gain_x_In_Over_Abs_Gain_x_In);

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::HardClip(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
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
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Gain_x_In = Gain * In_Plus_Bias;
            const VectorRegister4Float Gain_x_In = VectorMultiply(VGain, In_Plus_Bias);

            //float Out = FMath::Clamp(Gain_x_In, -1.0f, 1.0f);
            VectorRegister4Float Out = FSaturationUtils::VectorClampMinusOneToOne(Gain_x_In);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix      = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::Foldback(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Gain     = GainParamSmoother.GetValue();
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //    const float Two_x_Gain   = 2.0f * Gain;
        //    
        //    float Out = (In_Plus_Bias > Gain) ? Two_x_Gain - In_Plus_Bias
        //                                      : (In_Plus_Bias < -Gain) ? -Two_x_Gain - In_Plus_Bias
        //                                                               : In_Plus_Bias;
        //
        //    Out = FMath::Clamp(Out, -1.0f, 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentGain     = GainParamSmoother.GetValue();
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VGain     = VectorLoadFloat1(&CurrentGain);
            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //const float Two_x_Gain = 2.0f * Gain;
            const VectorRegister4Float Two_x_Gain = VectorMultiply(FSaturationUtils::VTwos, VGain);

            //float Out = (In_Plus_Bias > Gain) ? Two_x_Gain - In_Plus_Bias
            //                                  : (In_Plus_Bias < -Gain) ? -Two_x_Gain - In_Plus_Bias
            //                                                           : In_Plus_Bias;
            VectorRegister4Float Out = (VectorAnyGreaterThan(In_Plus_Bias, VGain) > 0) ? VectorSubtract(Two_x_Gain, In_Plus_Bias)
                                                                                       : (VectorAnyGreaterThan(VectorMultiply(FSaturationUtils::VMinusOnes, VGain), In_Plus_Bias) > 0) ? VectorSubtract(VectorMultiply(FSaturationUtils::VMinusOnes, Two_x_Gain), In_Plus_Bias)
                                                                                                                                                                                       : In_Plus_Bias;

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::HalfWaveRectifier(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //
        //    float Out = (In_Plus_Bias > 0.0f) ? In_Plus_Bias : 0.0f;
        //    Out = FMath::Clamp(Out, -1.0f, 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        { 
            const float CurrentBias     = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix      = MixParamSmoother.GetValue();

            const VectorRegister4Float VBias     = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix      = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //float Out = (In_Plus_Bias > 0.0f) ? In_Plus_Bias : 0.0f;
            VectorRegister4Float Out = (VectorAnyGreaterThan(In_Plus_Bias, FSaturationUtils::VZeros) > 0) ? In_Plus_Bias : FSaturationUtils::VZeros;

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }

    void FSaturation::FullWaveRectifier(const float* InBuffer, float* OutBuffer, int32 NumSamples)
    {
        // Naive version
        //for (int32 i = 0; i < NumSamples; ++i)
        //{
        //    const float Bias     = BiasParamSmoother.GetValue();
        //    const float OutLevel = OutLevelParamSmoother.GetValue();
        //    const float Mix      = MixParamSmoother.GetValue();
        //
        //    const float In = InBuffer[i];
        //
        //    const float In_Plus_Bias = In + Bias;
        //
        //    float Out = FMath::Abs(In_Plus_Bias);
        //    Out = FMath::Clamp(Out, -1.0f, 1.0f);
        //
        //    Out = Out * Mix + (1.0f - Mix) * In;
        //    Out = Out * OutLevel;
        //
        //    OutBuffer[i] = Out;
        //}

        // SIMD version
        for (int32 i = 0; i < NumSamples; i += 4)
        {
            const float CurrentBias = BiasParamSmoother.GetValue();
            const float CurrentOutLevel = OutLevelParamSmoother.GetValue();
            const float CurrentMix = MixParamSmoother.GetValue();

            const VectorRegister4Float VBias = VectorLoadFloat1(&CurrentBias);
            const VectorRegister4Float VOutLevel = VectorLoadFloat1(&CurrentOutLevel);
            const VectorRegister4Float VMix = VectorLoadFloat1(&CurrentMix);

            //const float In = InBuffer[i];
            const VectorRegister4Float In = VectorLoadAligned(&InBuffer[i]);

            //const float In_Plus_Bias = InBuffer[i] + Bias;
            const VectorRegister4Float In_Plus_Bias = VectorAdd(In, VBias);

            //float Out = FMath::Abs(In_Plus_Bias);
            VectorRegister4Float Out = VectorAbs(In_Plus_Bias);

            //Out = FMath::Clamp(Out, -1.0f, 1.0f);
            Out = FSaturationUtils::VectorClampMinusOneToOne(Out);

            //Out = Out * Mix + (1.0f - Mix) * In;
            const VectorRegister4Float One_Minus_Mix = VectorSubtract(FSaturationUtils::VOnes, VMix);
            const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
            Out = VectorMultiplyAdd(Out, VMix, One_Minus_Mix_x_In);

            //Out = Out * OutputLevel;
            Out = VectorMultiply(Out, VOutLevel);

            //OutBuffer[i] = Out;
            VectorStoreAligned(Out, &OutBuffer[i]);
        }
    }
}
