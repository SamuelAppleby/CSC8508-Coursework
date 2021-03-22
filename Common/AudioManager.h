#pragma once
#include <windows.h>
#include <irrKlang.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <vector>
#include "Vector3.h"
using namespace irrklang;
using namespace NCL;
using namespace Maths;

class AudioManager
{
public:
	AudioManager();
	void SetPlayerPos(Vector3 PlayerPos);
	void PlayAudio(std::string dir, bool loop = false);
	void UpdateAudio(float dt);
	void StopSound();
	void Play3DAudio(std::string dir, vec3df objPos, bool loop);

	static int& GetVolume()
	{
		return volume;
	}
private:
	ISoundEngine* engine;
	ISound* music;
	vec3df ListenerPos;
	static int volume;
};

