#pragma once

#include "DSPProcessing/Saturation.h"
#include "Sound/SoundEffectSubmix.h"

#include "SubmixEffectSaturation.generated.h"


//////////////////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ESubmixEffectSaturationType : uint8
{
	Tape = 0,
	Tape2,
	Overdrive,
	Tube,
	Tube2,
	Distortion,
	Metal,
	Fuzz,
	HardClip,
	Foldback,
	HalfWaveRectifier,
	FullWaveRectifier,
	Count UMETA(Hidden)
};

//////////////////////////////////////////////////////////////////////////////////////

class AUDIODSPCOLLECTION_API FSubmixEffectSaturation : public FSoundEffectSubmix
{
public:
	virtual ~FSubmixEffectSaturation() = default;

	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSubmixInitData& InitData) override;

	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio thread.
	virtual void OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData) override;

protected:
	DSPProcessing::FSaturation SaturationDSPProcessor;
};

//////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct AUDIODSPCOLLECTION_API FSubmixEffectSaturationSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float Gain = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", meta = (ClampMin = "-1.0", ClampMax = "1.0", UIMin = "-1.0", UIMax = "1.0"))
	float Bias = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float Mix = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", meta = (ClampMin = "-96.0", ClampMax = "24.0", UIMin = "-96.0", UIMax = "24.0"))
	float OutLevelDb = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset")
	ESubmixEffectSaturationType SaturationType = ESubmixEffectSaturationType::Tape;
};

//////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class AUDIODSPCOLLECTION_API USubmixEffectSaturationPreset : public USoundEffectSubmixPreset
{
	GENERATED_BODY()

public:
	EFFECT_PRESET_METHODS(SubmixEffectSaturation)
	
	UFUNCTION(BlueprintCallable, Category = "Audio|Effects|Saturation")
	void SetSettings(const FSubmixEffectSaturationSettings& InSettings);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", Meta = (ShowOnlyInnerProperties))
	FSubmixEffectSaturationSettings Settings;
};

//////////////////////////////////////////////////////////////////////////////////////
