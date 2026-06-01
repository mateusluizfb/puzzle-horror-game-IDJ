#include "Teleporter.h"
#include "Character.h"
#include "Collider.h"
#include "Log.h"
#include "MazeState.h"
#include "Game.h"

Teleporter::Teleporter(GameObject& associated, Vec2 destination, bool completesLoop)
: Component(associated), destination(destination), completesLoop(completesLoop) {

	Collider* collider = new Collider(associated);
	associated.AddComponent(collider);
}

void Teleporter::Start() {
	// Adiciona o colisor fisico para sabermos quando o jogador pisar
}

void Teleporter::Update(float dt) {}
void Teleporter::Render() {}

void Teleporter::NotifyCollision(GameObject& other) {
	// Se quem pisou foi o heroi
	if (other.tag == "player") {
		Log::info("[TELEPORTER] Teletransportando jogador...");

		// Teletransporte
		other.box.x = destination.x;
		float distanciaDaParede = other.box.y - associated.box.y;
        other.box.y = destination.y + distanciaDaParede;

		// Se este for o teleporte que leva de volta ao Corredor A,
		// avisamos o Labirinto que um Loop inteiro foi concluido
		if (completesLoop) {
			State& currentState = Game::GetInstance().GetCurrentState();

			// Usamos o dynamic_cast para converter o State generico no nosso MazeState
			MazeState* maze = dynamic_cast<MazeState*>(&currentState);
			if (maze != nullptr) maze->AddLoop();
		}
	}
}
