#pragma once

#include "DSPProcessing/Helpers/ParamSmoother.h"


namespace DSPProcessing
{
	class AUDIODSPCOLLECTION_API FGain
	{
	public:
		void InitGainParam(float SmoothingTimeInMs, float SampleRate);

		void SetGain(float InGain);

		void ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, int32 NumSamples);

	private:
		FORCEINLINE void ProcessGain(const float* InBuffer, float* OutBuffer, int32 NumSamples);

		ParamSmootherLPF GainParamSmoother;
	};
}
