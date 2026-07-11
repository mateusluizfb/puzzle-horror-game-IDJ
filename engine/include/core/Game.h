#ifndef GAME_H
#define GAME_H

#define INCLUDE_SDL_IMAGE
#define INCLUDE_SDL_MIXER
#define INCLUDE_SDL_TTF
#define INCLUDE_SDL
#include "SDL_include.h"

#include <string>
#include <stack>

#include "State.h"

class Game
{
private:
  static Game* instance;
  static SDL_Renderer* renderer;
  static SDL_Window* window;

  int frameStart;
  float dt;

  State* storedState;
  std::stack<std::unique_ptr<State>> stateStack;

  Game(const std::string &title, int width, int height);
  void CalculateDeltaTime();

public:
  ~Game();

  static Game& GetInstance(const std::string &title = "", int width = 0, int height = 0);
  static SDL_Renderer *GetRenderer();

  State& GetCurrentState();
  void Push(State* state);
  void StateStackPush(State* state);

  float GetDeltaTime();

  int GetWindowWidth();
  int GetWindowHeight();

  void Run();
};

#endif
