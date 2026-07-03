#ifndef SOUND_H
#define SOUND_H

#include <string>
#include "SDL_mixer.h"

class Sound {
private:
  Mix_Chunk* chunk;
  int channel;

public:
  Sound();
  Sound(std::string file);
  ~Sound();

  void Play(int times = 1);
  void Stop();
  void Open(std::string file);
  bool IsOpen();
  void SetVolume(int volume);
};

#endif
