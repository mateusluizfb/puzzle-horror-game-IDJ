#include "Door.h"
#include "Character.h"
#include "Collider.h"
#include "Log.h"
#include "MazeState.h"
#include "Game.h"
#include "EndState.h"

Door::Door(GameObject& associated, Vec2 destination, Vec2 penaltyDest, std::string side, bool isWinCondition, bool isDeathTrap)
: Component(associated), destination(destination), penaltyDest(penaltyDest), side(side), isCorrect(false), isWinCondition(isWinCondition), isDeathTrap(isDeathTrap) {
	Collider* collider = new Collider(associated);
	associated.AddComponent(collider);
}

void Door::Start() {}
void Door::Update(float dt) {}
void Door::Render() {}

void Door::NotifyCollision(GameObject& other) {
	if (other.tag == "player") {
		Character* character = other.GetComponent<Character>();
		if (character == nullptr) return;

		State& currentState = Game::GetInstance().GetCurrentState();
		MazeState* maze = dynamic_cast<MazeState*>(&currentState);
		if (maze == nullptr) return;

		// O Salto de Fe (Condicao de Vitoria)
		if (isWinCondition) {
			Log::info("[DOOR] A FÉ FOI RECOMPENSADA. O labirinto foi vencido!");
			GameData::playerVictory = true;
			maze->RequestPop();
			Game::GetInstance().Push(new EndState());
			return;
		}

		// A Armadilha Final (Condicao de Morte Subita)
		if (isDeathTrap) {
			Log::warning("[DOOR] A ISCA FOI MORDIDA. Morte instantânea.");
			GameData::playerLives = 0;
			character->TakeDamage();
			return;
		}

		// Portas do Enigma (O Cruzamento em "T")
		if (isCorrect) {
			Log::info("[DOOR] Porta CORRETA escolhida (" + side + "). Avancando!");
			maze->AddLoop();

			// Teletransporta para o proximo desafio
			other.box.x = destination.x;
			other.box.y = destination.y;
		} else {
			Log::warning("[DOOR] Porta ERRADA (" + side + "). Castigando o jogador.");
			character->TakeDamage();

			if (GameData::playerLives > 0) {
				maze->ResetLoop(); // Zera o progresso e randomiza a resposta
				other.box.x = penaltyDest.x;
                other.box.y = penaltyDest.y;
			}
		}
	}
}
