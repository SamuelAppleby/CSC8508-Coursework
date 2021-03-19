#include "AudioManager.h"
#include <iostream>
int AudioManager::volume = 0;

AudioManager::AudioManager() {
	volume = 25.0f;
	engine = createIrrKlangDevice();
}

void AudioManager::PlayAudio(std::string dir, bool loop) {
	engine->play2D(dir.c_str(), loop);
}

void AudioManager::UpdateAudio(float dt) {
	engine->setSoundVolume((float)volume / 100);
}

void AudioManager::StopSound() {
	engine->stopAllSounds();
}
