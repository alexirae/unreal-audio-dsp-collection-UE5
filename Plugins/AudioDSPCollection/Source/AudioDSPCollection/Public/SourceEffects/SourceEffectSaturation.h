#pragma once

#include "DSPProcessing/Saturation.h"
#include "Sound/SoundEffectSource.h"

#include "SourceEffectSaturation.generated.h"


//////////////////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ESourceEffectSaturationType : uint8
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

class AUDIODSPCOLLECTION_API FSourceEffectSaturation : public FSoundEffectSource
{
public:
	virtual ~FSourceEffectSaturation() = default;

	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSourceInitData& InitData) override;

	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio thread.
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;

protected:
	DSPProcessing::FSaturation SaturationDSPProcessor;
	int32 NumChannels;
};

//////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct AUDIODSPCOLLECTION_API FSourceEffectSaturationSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float Gain = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset", meta = (ClampMin = "-1.0", ClampMax = "1.0", UIMin = "-1.0", UIMax = "1.0"))
	float Bias = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float Mix = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset", meta = (ClampMin = "-96.0", ClampMax = "24.0", UIMin = "-96.0", UIMax = "24.0"))
	float OutLevelDb = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset")
	ESourceEffectSaturationType SaturationType = ESourceEffectSaturationType::Tape;
};

//////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class AUDIODSPCOLLECTION_API USourceEffectSaturationPreset : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	EFFECT_PRESET_METHODS(SourceEffectSaturation)

	UFUNCTION(BlueprintCallable, Category = "Audio|Effects|Saturation")
	void SetSettings(const FSourceEffectSaturationSettings& InSettings);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SourceEffect|Preset", Meta = (ShowOnlyInnerProperties))
	FSourceEffectSaturationSettings Settings;
};

//////////////////////////////////////////////////////////////////////////////////////
