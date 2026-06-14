#include "Text.h"
#include "GameObject.h"
#include "Game.h"
#include "Camera.h"
#include "Log.h"

Text::Text(GameObject &associated,
               std::string fontFile,
               int fontSize,
               TextStyle style,
               std::string text,
               SDL_Color color)
    : Component(associated),
      font(nullptr),
      texture(nullptr),
      text(text),
      style(style),
      fontFile(fontFile),
      fontSize(fontSize),
      color(color)
{ 
  RemakeTexture();
}

Text::~Text() {
  if (font) {
    TTF_CloseFont(font);
  }
  if (texture) {
    SDL_DestroyTexture(texture);
  }
}

void Text::Update(float dt) {}

void Text::Render() {
  if (texture && !hidden) {
    SDL_Rect dstRect;
    dstRect.x = associated.box.x;
    dstRect.y = associated.box.y;

    SDL_QueryTexture(texture, nullptr, nullptr, &dstRect.w, &dstRect.h);
    SDL_RenderCopyEx(
      Game::GetRenderer(),
      texture,
      nullptr,
      &dstRect,
      associated.angleDeg,
      nullptr,
      SDL_FLIP_NONE
    );
  }
}

void Text::SetText(std::string text) {
  this->text = text;
  RemakeTexture();
}

void Text::SetColor(SDL_Color color) {
  this->color = color;
  RemakeTexture();
}

void Text::SetStyle(TextStyle style) {
  this->style = style;
  RemakeTexture();
}

void Text::SetFontFile(std::string fontFile) {
  this->fontFile = fontFile;
  RemakeTexture();
}

void Text::SetFontSize(int fontSize) {
  this->fontSize = fontSize;
  RemakeTexture();
}

void Text::Show() {
  this->hidden = false;
}

void Text::Hide() {
  this->hidden = true;
}

void Text::RemakeTexture() {
  Log::info("TEXT - Remaking text texture");

  if (texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }

  if (font) {
    TTF_CloseFont(font);
    font = nullptr;
  }

  font = TTF_OpenFont(fontFile.c_str(), fontSize);
  if (!font) {
    Log::error("TEXT - Failed to load font: " + fontFile + " Error: " + TTF_GetError());
    return;
  }

  SDL_Surface* surface = nullptr;
  switch (style) {
    case SOLID:
      surface = TTF_RenderText_Solid(font, text.c_str(), color);
      break;
    case SHADED: {
      SDL_Color bgColor = {0, 0, 0, 255}; // Black background
      surface = TTF_RenderText_Shaded(font, text.c_str(), color, bgColor);
      break;
    }
    case BLENDED:
      surface = TTF_RenderText_Blended(font, text.c_str(), color);
      break;
  }

  Log::debug("TEXT - Created text surface");

  if (!surface) {
    Log::error("TEXT - Failed to create text surface: " + std::string(TTF_GetError()));
    return;
  }

  texture = SDL_CreateTextureFromSurface(Game::GetRenderer(), surface);
  SDL_FreeSurface(surface);

  if (!texture) {
    Log::error("TEXT - Failed to create text texture: " + std::string(SDL_GetError()));
    return;
  }

  Log::debug("TEXT - Created text texture successfully");

  // Update the associated box size
  int w, h;
  SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
  associated.box.w = w;
  associated.box.h = h;
}
