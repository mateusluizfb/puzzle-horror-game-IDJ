#pragma once
#include "Component.h"

class Saint : public Component {
	public:
		Saint(GameObject& associated);

		void Update(float dt) override;
		void Render() override;
};
