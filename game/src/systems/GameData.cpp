#include "GameData.h"

bool GameData::playerVictory = false;
Vec2 GameData::tileScaleClose = Vec2(4.0f, 4.0f);
Vec2 GameData::tileScaleFar = Vec2(2.0f, 2.0f);
bool GameData::dialogueActive = false;
int GameData::playerLives = 3;
bool GameData::hasLivingRoomKey = false;
bool GameData::hasBedroomKey = false;
void* GameData::activeInteraction = nullptr;
int GameData::interactionPriority = 0;
