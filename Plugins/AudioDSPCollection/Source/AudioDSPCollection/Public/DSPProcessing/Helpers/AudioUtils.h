#include "Math/UnrealMathSSE.h"

namespace DSPProcessing
{
	namespace AudioUtils
	{
		constexpr float Eps = 1.e-8f;

		FORCEINLINE float MapFromNormalizedRange(float value, float minRange, float maxRange)
		{
			return minRange + value * (maxRange - minRange);
		}

		// Vectorized vars/functions
		const VectorRegister4Float& VMinusOnes = GlobalVectorConstants::FloatMinusOne;
		const VectorRegister4Float& VZeros     = GlobalVectorConstants::FloatZero;
		const VectorRegister4Float& VOneHalf   = GlobalVectorConstants::FloatOneHalf;
		const VectorRegister4Float& VOnes      = GlobalVectorConstants::FloatOne;
		const VectorRegister4Float& VTwos      = GlobalVectorConstants::FloatTwo;
		const VectorRegister4Float& VEps       = GlobalVectorConstants::SmallLengthThreshold;

		constexpr VectorRegister4Float VThrees = MakeVectorRegisterFloatConstant(3.0f, 3.0f, 3.0f, 3.0f);

		FORCEINLINE VectorRegister4Float VectorClampMinusOneToOne(const VectorRegister4Float& Vec)
		{
			//Out = FMath::Clamp(Out, -1.0f, 1.0f);
			return VectorMax(VectorMin(Vec, VOnes), VMinusOnes);
		}

		FORCEINLINE void VectorMix(const VectorRegister4Float& In, const VectorRegister4Float& MixAmount, VectorRegister4Float& Out)
		{
			//Out = Out * Mix + (1.0f - Mix) * In;
			const VectorRegister4Float One_Minus_Mix      = VectorSubtract(VOnes, MixAmount);
			const VectorRegister4Float One_Minus_Mix_x_In = VectorMultiply(One_Minus_Mix, In);
			Out = VectorMultiplyAdd(Out, MixAmount, One_Minus_Mix_x_In);
		}
	}
}