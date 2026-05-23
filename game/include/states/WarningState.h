#pragma once
#include "Camera.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Resources.h"
#include "State.h"
#include "Text.h"
#include "Timer.h"
#include "Log.h"

class WarningState : public State {
	private:
		Timer displayTimer;

	public:
		WarningState();
		~WarningState();

		void LoadAssets() override;
		void Update(float dt) override;
		void Render() override;
		void Start() override;
		void Pause() override;
		void Resume() override;
};
