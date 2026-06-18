#pragma once

#include "Camera.h"
#include "CollisionSystem.h"
#include "DarkZone.h"
#include "Door.h"
#include "GameData.h"
#include "GameObject.h"
#include "Music.h"
#include "Resources.h"
#include "Saint.h"
#include "State.h"
#include "SpriteRenderer.h"
#include <algorithm>
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
		void ResetLoop();
        void ShuffleDoors();
		int GetLoopCount() const;
};
