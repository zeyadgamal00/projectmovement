#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2\SDL.h>
#include <SDL2\SDL_mixer.h>
#include <iostream>
#include<vector>
using namespace std;
vector<Mix_Chunk*> sounds(15);
vector<Mix_Music*> music;
extern float masterVolume, musicVolume, sfxVolume;
int slowmochannel=-1;
void initmixer() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		cerr << "Couldn't Initialize SDL: " << SDL_GetError()<<"\n";
	}
	if (Mix_Init( MIX_INIT_OGG | MIX_INIT_WAVPACK) < 0) {
		cerr << "Couldn't Initialize Mixer: " << Mix_GetError() << "\n";
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		cerr << "Mix_OpenAudio Error: " << Mix_GetError() << "\n";
		SDL_Quit();
	}
}
void loadMusic(const char* location) {
	music.push_back(Mix_LoadMUS(location));
	if (music.back() == NULL) {
		cerr << "Couldn't load music file: " << Mix_GetError() << "\n";
	}
}
void loadsound(const char* location,int index) {
	sounds[index]=Mix_LoadWAV(location);
	if (sounds[index] == NULL) {
		cerr << "Couldn't load music file: " << Mix_GetError() << "\n";
	}
}
void playMusic(int index,int repeat=-1) {
	Mix_VolumeMusic( MIX_MAX_VOLUME * musicVolume * masterVolume);
	if (music[index]) {
		Mix_PlayMusic(music[index], repeat);
	}
}
int playSound(int index,int repeat=0) {
	Mix_VolumeChunk(sounds[index], MIX_MAX_VOLUME * sfxVolume * masterVolume);
	if (sounds[index]) {

		return Mix_PlayChannel(-1, sounds[index], repeat);
	}
}
void changeVolume() {
	Mix_VolumeMusic(MIX_MAX_VOLUME * musicVolume * masterVolume);
	Mix_Volume(-1, MIX_MAX_VOLUME * sfxVolume * masterVolume);
}
void quitmixer() {
	for (int i = 0; i < music.size();i++) {
		if (music[i]) {
			Mix_FreeMusic(music[i]);
		}
		music[i] = NULL;
	}
	for (int i = 0; i < sounds.size(); i++) {
		if (sounds[i]) {
			Mix_FreeChunk(sounds[i]);
		}
		sounds[i] = NULL;
	}
	Mix_CloseAudio();
	SDL_Quit();
}
void soundsloader() { //loads commonly used sounds + music
	loadMusic("../Sounds/mainmenumusic.ogg");
	loadsound("../Sounds/HeartBeat.wav",6);
	loadMusic("../Sounds/gamemusic.ogg");
	loadMusic("../Sounds/pausemusic.ogg");
	loadsound("../Sounds/Swing2.wav",0);
	loadsound("../Sounds/Swing1.wav",1);
	loadsound("../Sounds/9mm.wav",2);
	loadsound("../Sounds/Uzi.wav",3);
	loadsound("../Sounds/Shotgun.wav",4);
	loadsound("../Sounds/PickUpWeapon.wav",5);
}
void slowmotionsoundhandler() {
	if (slowmo <=0.5) {
		if (slowmochannel == -1) {
			Mix_PauseMusic();
			slowmochannel = playSound(6, -1);
			cout << "test" << slowmo << endl << slowmochannel << endl;
		}
	}
	else {
		if (slowmochannel != -1) {
			Mix_HaltChannel(slowmochannel);
			Mix_ResumeMusic();
			slowmochannel = -1;
		}
	}
}