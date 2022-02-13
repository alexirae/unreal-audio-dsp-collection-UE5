#pragma once


namespace DSPProcessing
{
    class AUDIODSPCOLLECTION_API ParamSmootherLPF
    {
    public:
        ParamSmootherLPF();
        virtual ~ParamSmootherLPF();

        void Init(int32 InTransitionTimeInMs, float SampleRate);
        void SetNewParamValue(float InNewParamValue);
        FORCEINLINE float GetValue();

    private:
        FORCEINLINE float SmoothedResult();
        FORCEINLINE float DirectResult();

        float Step;
        float NewParamValue;
        float CurrentValue;
        bool  FirstTime;

        // Function pointer that points to either SmoothedResult or DirectResult
        float (ParamSmootherLPF::* SmoothedValueFuncPtr)();
    };
}
