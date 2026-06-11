#ifndef SPRITE_H
#define SPRITE_H

#define INCLUDE_SDL
#include "SDL_include.h"

#include <string>
#include "Vec2.h"

class Sprite
{
private:
  SDL_Texture* texture;
  SDL_Rect clipRect;
  SDL_RendererFlip flip;

  Vec2 scale;
  int width, height;
  int frameCountW, frameCountH;

public:
  bool cameraFollower;

  Sprite();
  Sprite(const std::string &file, int frameCountW = 1, int frameCountH = 1);
  ~Sprite();

  void Open(const std::string &file);
  void SetClip(int x, int y, int w, int h);
  void SetPosition(int x, int y);
  void Render(int x, int y, int w, int h, float angle = 0.0f);
  int GetWidth();
  int GetHeight();
  bool IsOpen();
  void SetFrame(int frame);
  void SetFrameCount(int frameCountW, int frameCountH);
  int GetFrameCount();
  int GetFrameW();
  int GetFrameH();
  void SetScale(float scaleX, float scaleY);
  Vec2 GetScale();
  void SetFlip(SDL_RendererFlip flip);
  SDL_RendererFlip GetFlip();
  SDL_Texture* GetTexture();
};

#endif
