#include "DSPProcessing/Helpers/ParamSmoother.h"

namespace DSPProcessing
{
	ParamSmootherLPF::ParamSmootherLPF()
		: SmoothedValueFuncPtr(&ParamSmootherLPF::DirectResult)
	{
		
	}

	ParamSmootherLPF::~ParamSmootherLPF()
	{

	}

	void ParamSmootherLPF::Init(int32 InTransitionTimeInMs, float SampleRate)
	{
		const float TransitionTimeInSamples = InTransitionTimeInMs * SampleRate * 0.001; // ms to samples

		Step = 1.0f - FMath::Exp(-UE_TWO_PI / TransitionTimeInSamples);
	}

	void ParamSmootherLPF::SetNewParamValue(float InNewParamValue)
	{
		static constexpr float Epsilon = 1.58489e-05f; // -96dB

		const bool AreValuesEqual = FMath::Abs(InNewParamValue - CurrentValue) < Epsilon;

		if (AreValuesEqual || FirstTime)
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
