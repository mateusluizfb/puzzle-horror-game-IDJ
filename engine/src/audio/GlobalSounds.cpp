#include "GlobalSounds.h"

Sound GlobalSounds::doorSound;
Sound GlobalSounds::buttonSound;
Sound GlobalSounds::keySound;

Sound& GlobalSounds::Door() {
  if (!doorSound.IsOpen()) {
    doorSound.Open("game/assets/music/door.mp3");
  }
  return doorSound;
}

Sound& GlobalSounds::Button() {
  if (!buttonSound.IsOpen()) {
    buttonSound.Open("game/assets/music/button.mp3");
    buttonSound.SetVolume(48);
  }
  return buttonSound;
}

Sound& GlobalSounds::Key() {
  if (!keySound.IsOpen()) {
    keySound.Open("game/assets/music/key.mp3");
    keySound.SetVolume(48);
  }
  return keySound;
}
