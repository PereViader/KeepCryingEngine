#include "AudioSource.h"

#include <bass.h>
#include <imgui.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "Transform.h"

using namespace std;

AudioSource::AudioSource():Component(AudioSource::TYPE)
{
}

AudioSource::~AudioSource()
{}

void AudioSource::Awake()
{}

void AudioSource::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(audioInfo == nullptr)
	{
		return;
	}

	if(reloadId)
	{
		switch(audioInfo->type)
		{
			case SoundType::MUSIC:
			{
				HSAMPLE temp = App->audio->GetMusic(audioInfo->id);
				id = BASS_SampleGetChannel(temp, FALSE);
				int a = BASS_ErrorGetCode();
				int esperate = 0;
			}
			break;
			case SoundType::SFX:
			{
				id = App->audio->GetSFX(audioInfo->id);
			}
			break;
			default:
				assert(false);
				break;
		}
		reloadId = false;
	}

	if(id == 0)
	{
		return;
	}

	switch(state)
	{
		case SourceStates::PLAYING:
		{
			BASS_ChannelSet3DAttributes(id, BASS_3DMODE_NORMAL, 0, maxDistance, -1, -1, -1);

			// Update 3D position

			Transform* body = gameObject->GetTransform();

			BASS_ChannelSet3DPosition(id,
				(BASS_3DVECTOR*)&body->GetWorldPosition(), // position
				(BASS_3DVECTOR*)&body->Forward(), // front
				nullptr); // velocity
		}
			break;
		case SourceStates::WAITING_TO_PLAY:
		{
			if(BASS_ChannelPlay(id, FALSE) == FALSE)
			{
				//LOG_DEBUG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, 0.0f);
				BASS_ChannelSlideAttribute(id, BASS_ATTRIB_VOL, 1.0f, DWORD(fadeIn * 1000.0f));
				state = SourceStates::PLAYING;
			}
		}
			break;
		case SourceStates::WAITING_TO_STOP:
		{
			if(BASS_ChannelStop(id) == FALSE)
			{
				//LOG("BASS_ChannelStop() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				BASS_ChannelSlideAttribute(id, BASS_ATTRIB_VOL, 0.0f, DWORD(fadeOut * 1000.0f));
				state = SourceStates::STOPPED;
			}
		}
			break;
		case SourceStates::WAITING_TO_PAUSE:
		{
			if(BASS_ChannelPause(id) == FALSE)
			{
				//LOG("BASS_ChannelPause() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				state = SourceStates::PAUSED;
			}
		}
			break;
		case SourceStates::WAITING_TO_UNPAUSE:
		{
			if(BASS_ChannelPlay(id, FALSE) == FALSE)
			{
				//LOG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				state = SourceStates::PLAYING;
			}
		}
			break;
	}
}

void AudioSource::DrawUI()
{
	if(ImGui::CollapsingHeader("Audio Source"))
	{
		static char pathToAudio[180] = "";

		ImGui::InputText("##PathToAudio", pathToAudio, sizeof(pathToAudio)); ImGui::SameLine();
		if(ImGui::Button("Load audio"))
		{
			string pathAndName(pathToAudio);
			size_t found = pathAndName.find_last_of("/\\");
			size_t found2 = pathAndName.rfind(".");
			//App->audio->Load(pathAndName.substr(0, found) + "/", pathAndName.substr(0, found2-found), pathAndName.substr(found2+1) );
			audioInfo = App->audio->Load("Assets/sfx/", "wavSound", "wav");
			reloadId = true;
		}

		if(ImGui::Button("Play"))
		{			
			if(state == SourceStates::PAUSED)
			{
				state = SourceStates::WAITING_TO_UNPAUSE;
			}
			else
			{
				state = SourceStates::WAITING_TO_PLAY;
				reloadId = true;
			}
		}
		if(ImGui::Button("Pause"))
		{
			state = SourceStates::WAITING_TO_PAUSE;
		}
		if(ImGui::Button("Stop"))
		{
			state = SourceStates::WAITING_TO_STOP;
		}
	}
}

void AudioSource::SetMusic(AudioId* audioInfo)
{
	this->audioInfo = audioInfo;
}

void AudioSource::SetMode(AudioMode newMode)
{
	mode = newMode;
}

void AudioSource::SetPitch(float value)
{
	pitch = value;
}

void AudioSource::SetPan(float value)
{
	pan = value;
}

void AudioSource::SetMaxDistance(float value)
{
	maxDistance = value;
}

void AudioSource::SetRollOffFactor(float value)
{
	rollOffFactor = value;
}

void AudioSource::SetDoplerFactor(float value)
{
	doplerFactor = value;
}

AudioId* AudioSource::GetMusic() const
{
	return audioInfo;
}

AudioMode AudioSource::GetMode() const
{
	return mode;
}

float AudioSource::GetPitch() const
{
	return pitch;
}

float AudioSource::GetPan() const
{
	return pan;
}

float AudioSource::GetMaxDistance() const
{
	return maxDistance;
}

float AudioSource::GetRollOffFactor() const
{
	return rollOffFactor;
}

float AudioSource::GetDoplerFactor() const
{
	return doplerFactor;
}
