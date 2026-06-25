#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "Vec2.h"

class GameData {

  public:
    static bool playerVictory;
    static Vec2 tileScaleClose;
    static Vec2 tileScaleFar;
    static Vec2 tileScale;
    static bool dialogueActive;
    static int playerLives;
    static bool hasLivingRoomKey;
};

#endif
