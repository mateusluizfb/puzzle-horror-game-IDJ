#pragma once
#include "Component.h"
#include "Vec2.h"
#include <string>

class Door : public Component {
	private:
		Vec2 destination;
		Vec2 penaltyDest;

	public:
		std::string side;
		bool isCorrect;
		bool isWinCondition;
		bool isDeathTrap;

		Door(GameObject& associated, Vec2 destination, Vec2 penaltyDest, std::string side, bool isWinCondition = false, bool isDeathTrap = false);

		void Start() override;
		void Update(float dt) override;
		void Render() override;
		void NotifyCollision(GameObject& other) override;
};
