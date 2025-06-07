#pragma once

#include "DSPProcessing/Helpers/ParamSmoother.h"


namespace DSPProcessing
{
	class AUDIODSPCOLLECTION_API FGain
	{
	public:
		void Init(const float InSampleRate);

		void SetGain(const float InGain);

		void ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);

	private:
		FORCEINLINE void ProcessGain(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);

		ParamSmootherLPF GainParamSmoother;
	};
}
