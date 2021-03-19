#pragma once
#include <irrKlang.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <vector>
using namespace irrklang;

class AudioManager
{
public:
	AudioManager();
	void PlayAudio(std::string dir, bool loop = false);
	void UpdateAudio(float dt);
	void StopSound();
	static int& GetVolume() {
		return volume;
	}
private:
	ISoundEngine* engine;
	static int volume;
};

