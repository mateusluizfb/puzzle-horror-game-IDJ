#include <string>
#include <stdexcept>
#include "Log.h"
#include "Game.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Resources.h"
#include "InputManager.h"

Game* Game::instance = nullptr;
SDL_Renderer* Game::renderer = nullptr;
SDL_Window* Game::window = nullptr;

SDL_Window* init_window(const std::string &title, int width, int height)
{
  Log::info("GAME - Creating window: " + title + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");

  SDL_Window *window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
  if (window == nullptr)
  {
    throw std::runtime_error("GAME - Failed to create SDL window: " + std::string(SDL_GetError()));
  }

  return window;
}

SDL_Renderer* init_renderer(SDL_Window* window)
{
  Log::info("GAME - Creating renderer");

  SDL_Renderer *initialized_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (initialized_renderer == nullptr)
  {
    throw std::runtime_error("GAME - Failed to create SDL renderer: " + std::string(SDL_GetError()));
  }

  return initialized_renderer;
}

void init_sdl_libs()
{
  Log::info("GAME - Initializing SDL and its dependencies");

  int sdlInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
  if (sdlInitResult != 0)
  {
    throw std::runtime_error("GAME - Failed to initialize SDL: " + std::string(SDL_GetError()));
  }

  int imgInitResult = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
  if ((imgInitResult & (IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF)) == 0)
  {
    throw std::runtime_error("GAME - Failed to initialize SDL_image: " + std::string(IMG_GetError()));
  }

  int mixInitResult = Mix_Init(MIX_INIT_MP3);
  if ((mixInitResult & MIX_INIT_MP3) == 0)
  {
    throw std::runtime_error("GAME - Failed to initialize SDL_mixer: " + std::string(Mix_GetError()));
  }

  int mixOpenResult = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
  
  if (mixOpenResult != 0)
  {
    throw std::runtime_error("GAME - Failed to open audio: " + std::string(Mix_GetError()));
  }

  int allocateChannelsResult = Mix_AllocateChannels(32);
  if (allocateChannelsResult != 32)
  {
    throw std::runtime_error("GAME - Failed to allocate mixing channels: " + std::string(Mix_GetError()));
  }

  int ttfInitResult = TTF_Init();
  if (ttfInitResult != 0)
  {
    throw std::runtime_error("GAME - Failed to initialize SDL_ttf: " + std::string(TTF_GetError()));
  }

  Log::info("GAME - SDL and its dependencies initialized successfully.");
}

Game::Game(const std::string &title, int width, int height)
  : frameStart(0),
    dt(0.0),
    storedState(nullptr)
{
  Log::info("GAME - Initializing game: " + title + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");
  if (instance != nullptr)
  {
    throw std::runtime_error("Game instance already exists!");
  } else {
    instance = this;
  }
  
  init_sdl_libs();
  window = init_window(title, width, height);
  renderer = init_renderer(window);

  srand(time(NULL));
}

void Game::CalculateDeltaTime() {
  Uint32 currentTicks = SDL_GetTicks();

  this->dt = (currentTicks - frameStart) / 1000.0f;
  this->frameStart = currentTicks;
}

float Game::GetDeltaTime() {
  return this->dt;
}

Game::~Game()
{
  Log::info("GAME - Cleaning up game resources");

  if (storedState != nullptr) {
    delete storedState;
    storedState = nullptr;
  }

  while (!stateStack.empty()) {
    stateStack.pop();
  }

  Resources::ClearImages();
  Resources::ClearSounds();
  Resources::ClearMusics();

  instance = nullptr;

  TTF_Quit();
  Mix_CloseAudio();
  Mix_Quit();
  IMG_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  Log::info("GAME - Game resources cleaned up successfully.");
}

Game& Game::GetInstance(const std::string &title, int width, int height)
{
  if (instance == nullptr) {
    Log::info("GAME - Creating game instance");

    instance = new Game(title, width, height);
  }

  return *instance;
}

State &Game::GetCurrentState()
{
  return *stateStack.top().get();
}

SDL_Renderer *Game::GetRenderer()
{
  return renderer;
}

int Game::GetWindowWidth()
{
  int w;
  SDL_GetWindowSize(window, &w, nullptr);
  return w;
}

int Game::GetWindowHeight()
{
  int h;
  SDL_GetWindowSize(window, nullptr, &h);
  return h;
}

void Game::Push(State* state)
{
  Log::info("GAME - Storing new state to be pushed next frame");
  storedState = state;
}

void Game::StateStackPush(State* state)
{
  previousState = stateStack.empty() ? nullptr : stateStack.top().get();

  stateStack.push(std::unique_ptr<State>(state));
  stateStack.top()->Start();
  stateStack.top()->LoadAssets();
  storedState = nullptr;
}

void Game::Run()
{
  Log::info("GAME - Starting game loop");
  InputManager& inputManager = InputManager::GetInstance();

  if (storedState != nullptr) {
    this->StateStackPush(storedState);
  } else {
    Log::error("GAME - No initial state provided. Exiting.");
    return;
  }

  while (!stateStack.empty() && !stateStack.top()->QuitRequested()) {
    CalculateDeltaTime();
    inputManager.Update();

    if (stateStack.top()->PopRequested()) {
      Log::info("GAME - Popping current state");

      stateStack.pop();
      if (!stateStack.empty()) {
        Log::info("GAME - Resuming previous state");
        stateStack.top()->Resume();
      }
      continue;
    }
\
    if (storedState != nullptr) {
      Log::info("GAME - Pushing new state");

      stateStack.top()->Pause();
      this->StateStackPush(storedState);
    }

    if (stateStack.top()->isOverlay && previousState != nullptr) {
      previousState->Update(dt);
      previousState->Render();
    }

    stateStack.top()->Update(dt);
    stateStack.top()->Render();
    SDL_RenderPresent(renderer);
    SDL_Delay(33); // Force ~30 FPS
  }

  Log::info("GAME - Exiting game loop");
}
