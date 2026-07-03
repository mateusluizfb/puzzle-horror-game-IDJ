#include "Log.h"
#include "Sound.h"
#include "Resources.h"

Sound::Sound() : chunk(nullptr) {}

Sound::Sound(std::string file) : Sound() {
    Open(file);
}

void Sound::Play(int times) {
  if (chunk == nullptr)
  {
    throw std::runtime_error("SOUND - Can't play a sound that is not open.");
  }

  channel = Mix_PlayChannel(-1, chunk, 0);

  if (channel == -1)
  {
    throw std::runtime_error("SOUND - Failed to play sound: " + std::string(Mix_GetError()));
  }
}

void Sound::Stop() {
  if (chunk == nullptr)
  {
    throw std::runtime_error("SOUND - Can't stop a sound that is not open.");
  }

  Mix_HaltChannel(channel);
}

void Sound::Open(std::string file) {
  if (chunk != nullptr)
  {
    throw std::runtime_error("SOUND - Can't open a sound that is already open.");
  }

  chunk = Resources::GetSound(file.c_str());

  if (chunk == nullptr)
  {
    throw std::runtime_error("SOUND - Failed to load sound file: " + file + " - " + Mix_GetError());
  }
}

Sound::~Sound() {
  Log::info("SOUND - Destroying sound");

  chunk = nullptr;
}

void Sound::SetVolume(int volume) {
  if (chunk == nullptr)
  {
    throw std::runtime_error("SOUND - Can't set volume of a sound that is not open.");
  }

  Mix_VolumeChunk(chunk, volume);
}
