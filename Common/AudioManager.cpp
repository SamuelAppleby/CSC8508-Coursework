#include "AudioManager.h"
#include <iostream>
int AudioManager::volume = 0;
inline void sleepSomeTime() { Sleep(100); }

AudioManager::AudioManager()
{
	volume = 25.0f;
	engine = createIrrKlangDevice();

}

void AudioManager::PlayAudio(std::string dir, bool loop)
{
	engine->play2D(dir.c_str(), loop);
}

void AudioManager::UpdateAudio(float dt)
{
	engine->setSoundVolume((float)volume / 100);
}

void AudioManager::StopSound()
{
	engine->stopAllSounds();
}


void AudioManager::SetPlayerPos(Vector3  playerPos)
{
	vec3df pos = vec3df(playerPos.x, playerPos.y, playerPos.z);

	ListenerPos = pos;

	engine->setListenerPosition(pos, ListenerPos + vec3df(0, 0, 1));
}
//GameManager::GetAudioManager()->Play3DAudio("../../Assets/Audio/bell.wav",vec3df(0,10,0),true);
void AudioManager::Play3DAudio(std::string dir, vec3df objPos, bool loop)
{
	music = engine->play3D(dir.c_str(), objPos, loop);
}
