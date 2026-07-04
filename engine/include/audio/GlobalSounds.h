#ifndef GLOBAL_SOUNDS_H
#define GLOBAL_SOUNDS_H

#include "Sound.h"

class GlobalSounds {
private:
  static Sound doorSound;
  static Sound buttonSound;
  static Sound keySound;

public:
  static Sound& Door();
  static Sound& Button();
  static Sound& Key();
};

#endif
