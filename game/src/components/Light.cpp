#include "Light.h"
#include "Camera.h"
#include "Game.h"
#include <algorithm>

std::vector<Light*> Light::lights;

Light::Light(GameObject& associated, const std::string& file, float scale, Uint8 r, Uint8 g, Uint8 b, Uint8 a) : Component(associated), r(r), g(g), b(b), a(a) {
    sprite.Open(file);
    sprite.SetScale(scale, scale);

	// Modo Aditivo
    SDL_SetTextureBlendMode(sprite.GetTexture(), SDL_BLENDMODE_ADD);
    
    lights.push_back(this);
}

Light::~Light() {
	auto it = std::find(lights.begin(), lights.end(), this);
	if (it != lights.end())
		lights.erase(it);
}

void Light::Update(float dt) {}
void Light::Render() {}

void Light::RenderAll() {
	for (Light* light : lights) {
		float centerX = light->associated.box.x + light->associated.box.w / 2.0f;
		float centerY = light->associated.box.y + light->associated.box.h / 2.0f;

		float w = light->sprite.GetWidth();
		float h = light->sprite.GetHeight();

		float lightX = centerX - (w / 2.0f);
		float lightY = centerY - (h / 2.0f);

		// Aplica a cor na textura compartilhada, so para esta luz
		SDL_SetTextureColorMod(light->sprite.GetTexture(), light->r, light->g, light->b);
		SDL_SetTextureAlphaMod(light->sprite.GetTexture(), light->a);

		// Desenha a luz com a cor aplicada
		light->sprite.Render((int)lightX, (int)lightY, (int)w, (int)h);
	}
}
