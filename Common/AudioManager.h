#pragma once
#include <windows.h>
#include <irrKlang.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <vector>
#include "Vector3.h"
using namespace irrklang;

class AudioManager
{
public:
	AudioManager(); 
	void SetPlayerPos(vec3df PlayerPos);
	void PlayAudio(std::string dir, bool loop = false);
	void UpdateAudio(float dt);
	void StopSound();
	void Play3DAudio(std::string dir, const PxTransform& t, bool loop);

	static int& GetVolume() {
		return volume;
	}
private:
	ISoundEngine* engine;
	ISound* music;
	vec3df ListenerPos;
	static int volume;
};

