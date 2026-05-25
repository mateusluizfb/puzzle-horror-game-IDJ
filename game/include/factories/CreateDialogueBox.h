#ifndef CREATE_DIALOGUE_BOX_H
#define CREATE_DIALOGUE_BOX_H

#define INCLUDE_SDL_TTF
#define INCLUDE_SDL
#include "SDL_include.h"

#include "Rect.h"

#include <string>
#include <vector>

class GameObject;

GameObject* CreateDialogueBox(Rect box,
                              std::string fontFile,
                              int fontSize,
                              SDL_Color textColor,
                              std::string text,
                              std::vector<std::string> options);

#endif
