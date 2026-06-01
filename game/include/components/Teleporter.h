#pragma once
#include "Component.h"
#include "Vec2.h"

class Teleporter : public Component {
	private:
		Vec2 destination;
		bool completesLoop;

	public:
		Teleporter(GameObject& associated, Vec2 destination, bool completesLoop = false);

		void Start() override;
		void Update(float dt) override;
		void Render() override;
		void NotifyCollision(GameObject& other) override;
};
