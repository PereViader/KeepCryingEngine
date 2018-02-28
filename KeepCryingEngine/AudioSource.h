#ifndef _AUDIOSOURCE_H_
#define _AUDIOSOURCE_H_

#include "Component.h"
#include "ModuleAudio.h"

enum class AudioMode
{
	STEREO,
	MONO
};

enum class SourceStates
{
	PLAYING,
	WAITING_TO_PLAY,
	WAITING_TO_STOP,
	WAITING_TO_PAUSE,
	WAITING_TO_UNPAUSE,
	STOPPED,
	PAUSED
};

class AudioSource: public Component
{
public:
	static const Component::Type TYPE = Component::Type::AudioSource;
	AudioSource();
	~AudioSource();

	void Awake() override;
	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;

	void DrawUI() override;

	void SetMusic(AudioId* audioInfo);
	void SetMode(AudioMode newMode);
	void SetPitch(float value);
	void SetPan(float value);
	void SetMaxDistance(float value);
	void SetRollOffFactor(float value);
	void SetDoplerFactor(float value);

	AudioId* GetMusic() const;
	AudioMode GetMode() const;
	float GetPitch() const;
	float GetPan() const;
	float GetMaxDistance() const;
	float GetRollOffFactor() const;
	float GetDoplerFactor() const;
private:
	SourceStates state = SourceStates::STOPPED;
	AudioId* audioInfo = nullptr;
	AudioMode mode;
	float pitch;
	float pan;
	float maxDistance;
	float rollOffFactor;
	float doplerFactor;

	float fadeIn;
	float fadeOut;
};

#endif // !_AUDIOSOURCE_H_

