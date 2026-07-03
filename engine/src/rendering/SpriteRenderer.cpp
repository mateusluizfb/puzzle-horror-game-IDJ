#include "Log.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Sprite.h"

SpriteRenderer::SpriteRenderer(GameObject &associated)
: Component(associated), sprite(new Sprite()) {}

SpriteRenderer::SpriteRenderer(GameObject &associated, std::string file, int frameCountW, int frameCountH)
: Component(associated), sprite(new Sprite()) {
  this->Open(file);
  sprite->SetFrameCount(frameCountW, frameCountH);
  sprite->SetFrame(0);
  associated.box.w = sprite->GetFrameW();
  associated.box.h = sprite->GetFrameH();
}

void SpriteRenderer::Open(std::string file)
{
  Log::info("SPRITE_RENDERER - Opening sprite file: " + file);

  sprite->Open(file);
}

void SpriteRenderer::SetFrameCount(int frameCountW, int frameCountH)
{
  sprite->SetFrameCount(frameCountW, frameCountH);
}

void SpriteRenderer::SetFrame(int frame, SDL_RendererFlip flip)
{
  sprite->SetFlip(flip);
  sprite->SetFrame(frame);
}

void SpriteRenderer::SetPosition(int x, int y) {
  associated.box.x = x;
  associated.box.y = y;
}

void SpriteRenderer::SetCameraFollower(bool cameraFollower) {
  sprite->cameraFollower = cameraFollower;
}

void SpriteRenderer::Update(float dt) {}

void SpriteRenderer::Render() {
  if (hidden) return;

  sprite->Render(associated.box.x, associated.box.y, associated.box.w, associated.box.h, associated.angleDeg);
}

void SpriteRenderer::SetScale(float scaleX, float scaleY) {
  sprite->SetScale(scaleX, scaleY);

  associated.box.w = sprite->GetFrameW() * scaleX;
  associated.box.h = sprite->GetFrameH() * scaleY;
}

void SpriteRenderer::SetFlip(SDL_RendererFlip flip) {
  sprite->SetFlip(flip);
}

SDL_RendererFlip SpriteRenderer::GetFlip() {
  return sprite->GetFlip();
}

void SpriteRenderer::SetColorMod(Uint8 r, Uint8 g, Uint8 b) {
  if (sprite != nullptr && sprite->GetTexture() != nullptr)
    SDL_SetTextureColorMod(sprite->GetTexture(), r, g, b);
}
