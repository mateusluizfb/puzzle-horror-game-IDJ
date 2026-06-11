#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#define INCLUDE_SDL
#include "SDL_include.h"

#include <string>
#include "Component.h"

class SpriteRenderer : public Component {
private:
  class Sprite* sprite;

public:
  SpriteRenderer(GameObject &associated);
  SpriteRenderer(GameObject &associated, std::string file, int frameCountW = 1, int frameCountH = 1);

  void Open(std::string file);
  void SetFrameCount(int frameCountW, int frameCountH);
  void SetFrame(int frame, SDL_RendererFlip flip = SDL_FLIP_NONE);
  void SetPosition(int x, int y);
  void SetCameraFollower(bool cameraFollower);
  void SetScale(float scaleX, float scaleY);
  void SetFlip(SDL_RendererFlip flip);
  SDL_RendererFlip GetFlip();
  void SetColorMod(Uint8 r, Uint8 g, Uint8 b);

  void Update(float dt) override;
  void Render() override;
};

#endif
