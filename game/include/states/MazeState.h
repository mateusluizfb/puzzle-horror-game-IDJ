#pragma once

#include "State.h"
#include "Music.h"
#include "Camera.h"
#include "GameObject.h"
#include "GameData.h"
#include "Resources.h"
#include "SpriteRenderer.h"
#include "CollisionSystem.h"
#include <memory>
#include <vector>
#include "SDL2/SDL.h"

class MazeState : public State {
	private:
		Music music;
		CollisionSystem collisionSystem;
		SDL_Texture* lightMap;
		int loopCount;

	public:
		MazeState();
		~MazeState();

		void LoadAssets() override;
		void Start() override;
		void Pause() override;
		void Resume() override;

		void Update(float dt) override;
		void Render() override;

		void AddLoop();
		int GetLoopCount() const;
};
