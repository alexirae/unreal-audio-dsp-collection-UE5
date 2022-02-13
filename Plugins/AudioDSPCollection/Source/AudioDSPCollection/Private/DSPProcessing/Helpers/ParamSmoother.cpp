#include "DSPProcessing/Helpers/ParamSmoother.h"
#include "CoreMinimal.h"

namespace DSPProcessing
{
    ParamSmootherLPF::ParamSmootherLPF()
        : Step(0.0f)
        , NewParamValue(0.0f)
        , CurrentValue(0.0f)
        , FirstTime(true)
        , SmoothedValueFuncPtr(&ParamSmootherLPF::DirectResult)
    {
        
    }

    ParamSmootherLPF::~ParamSmootherLPF()
    {

    }

    void ParamSmootherLPF::Init(int32 InTransitionTimeInMs, float SampleRate)
    {
        const float TransitionTimeInSamples = InTransitionTimeInMs * SampleRate * 0.001; // ms to samples

        Step = 1.0f - FMath::Exp(-2.0f * PI / static_cast<float>(TransitionTimeInSamples));
    }

    void ParamSmootherLPF::SetNewParamValue(float InNewParamValue)
    {
        static constexpr float Epsilon = 1.58489e-05f; // -96dB

        const bool areValuesEqual = FMath::Abs(InNewParamValue - CurrentValue) < Epsilon;

        if (areValuesEqual || FirstTime)
        {
            FirstTime     = false;
            CurrentValue  = InNewParamValue;
            NewParamValue = InNewParamValue;

            SmoothedValueFuncPtr = &ParamSmootherLPF::DirectResult;

            return;
        }

        NewParamValue        = InNewParamValue;
        SmoothedValueFuncPtr = &ParamSmootherLPF::SmoothedResult;
    }

    float ParamSmootherLPF::GetValue()
    {
        return (this->*(SmoothedValueFuncPtr))();
    }

    float ParamSmootherLPF::SmoothedResult()
    {
        CurrentValue += Step * (NewParamValue - CurrentValue);

        return CurrentValue;
    }

    float ParamSmootherLPF::DirectResult()
    {
        return CurrentValue;
    }
}
