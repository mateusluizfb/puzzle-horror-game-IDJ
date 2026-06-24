#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#define INCLUDE_SDL
#include "SDL_include.h"

#include <unordered_map>

#define LEFT_ARROW_KEY SDLK_LEFT
#define RIGHT_ARROW_KEY SDLK_RIGHT
#define UP_ARROW_KEY SDLK_UP
#define DOWN_ARROW_KEY SDLK_DOWN
#define ESCAPE_KEY SDLK_ESCAPE
#define LEFT_MOUSE_BUTTON SDL_BUTTON_LEFT
#define SPACE_KEY SDLK_SPACE
#define E_KEY SDLK_e
#define Z_KEY SDLK_z
#define X_KEY SDLK_x
#define M_KEY SDLK_m
#define N_KEY SDLK_n
#define R_KEY SDLK_r

class InputManager {
private:
  bool mouseState[6];
  int mouseUpdate[6];

  std::unordered_map<int, bool> keyState;
  std::unordered_map<int, int> keyUpdate;

  bool quitRequested;
  int updateCounter;
  int mouseX;
  int mouseY;

public:
  InputManager();
  ~InputManager();

  static InputManager& GetInstance();

  void Update();

  bool KeyPress(int key);
  bool KeyRelease(int key);
  bool IsKeyDown(int key);

  bool MousePress(int button);
  bool MouseRelease(int button);
  bool IsMouseDown(int button);

  int GetMouseX();
  int GetMouseY();

  int GetMouseXWorld();
  int GetMouseYWorld();

  bool QuitRequested();
};

#endif
