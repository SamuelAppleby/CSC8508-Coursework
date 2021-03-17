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
	void StopSound();
private:
	ISoundEngine* engine;
};

