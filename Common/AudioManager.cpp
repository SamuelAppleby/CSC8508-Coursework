#include "AudioManager.h"

AudioManager::AudioManager()
{
	engine = createIrrKlangDevice();
}

void AudioManager::PlayAudio(std::string dir, bool loop)
{
	engine->setSoundVolume(0.25f);
	engine->play2D(dir.c_str(), loop);
}

void AudioManager::StopSound()
{
	engine->stopAllSounds();
}
