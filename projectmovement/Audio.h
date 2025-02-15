#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2\SDL.h>
#include <SDL2\SDL_mixer.h>
#include <iostream>
#include<vector>
using namespace std;
vector<Mix_Chunk*> sounds;
vector<Mix_Music*> music;
extern float masterVolume, musicVolume, sfxVolume;
void initmixer() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		cerr << "Couldn't Initialize SDL: " << SDL_GetError()<<"\n";
	}
	if (Mix_Init( MIX_INIT_OGG) < 0) {
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
	if (!music.back()) {
		cerr << "Couldn't load music file: " << Mix_GetError() << "\n";
	}
}
void loadsound(const char* location) {
	sounds.push_back(Mix_LoadWAV(location));
	if (!sounds.back()) {
		cerr << "Couldn't load music file: " << Mix_GetError() << "\n";
	}
}
void playMusic(int index) {
	Mix_VolumeMusic( MIX_MAX_VOLUME * musicVolume * masterVolume);
	if (music[index]) {
		Mix_PlayMusic(music[index], -1);
	}
}
void playSound(int index) {
	Mix_VolumeChunk(sounds[index], MIX_MAX_VOLUME * sfxVolume * masterVolume);
	if (sounds[index]) {
		Mix_PlayChannel(-1,sounds[index], 0);
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