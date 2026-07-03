#pragma once
#include "Component.h"
#include "Sprite.h"
#include <string>
#include <vector>

class Light : public Component {
	private:
		Sprite sprite;
		static std::vector<Light*> lights;
		Uint8 r, g, b, a;

	public:
		Light(GameObject& associated, const std::string& file, float scale = 1.0f, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255);
		~Light();

		void Update(float dt) override;
		void Render() override;

		static void RenderAll();
};
