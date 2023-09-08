#pragma once

#include "HAL/Platform.h"

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

		float Step          = 0.0f;
		float NewParamValue = 0.0f;
		float CurrentValue  = 0.0f;
		bool  FirstTime     = true;

		// Function pointer that points to either SmoothedResult or DirectResult
		float (ParamSmootherLPF::* SmoothedValueFuncPtr)();
	};
}
