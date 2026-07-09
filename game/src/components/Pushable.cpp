#include "Pushable.h"
#include "Log.h"
#include "GameObject.h"
#include "Game.h"
#include "Collider.h"
#include "Character.h"
#include "InputManager.h"
#include "Text.h"
#include "Wall.h"
#include "Collision.h"
#include "GlobalSounds.h"

#include <cmath>

Pushable::Pushable(GameObject& associated, float pushSpeed)
	: Component(associated),
	pushDirection(Vec2(0, 0)),
	pushSpeed(pushSpeed)
{
	this->associated.tag = "pushable";

	GameObject *textGameObject = new GameObject();
	SDL_Color white = {255, 255, 255, 255};
	pushText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32,
			Text::BLENDED, "Segure E para empurrar/puxar", white);
	pushText->Hide();
	textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
	textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
	associated.AddComponent(pushText);
}


void Pushable::Update(float dt) {
	InputManager& inputManager = InputManager::GetInstance();

	// Se estamos segurando uma caixa, mas o jogador soltou a tecla E, a libertamos.
	if (gluedPlayer != nullptr && !inputManager.IsKeyDown(E_KEY)) {
		Character* charComp = gluedPlayer->GetComponent<Character>();
		if (charComp) {
			charComp->isGlued = false;
			charComp->glueAxis = Character::GlueAxis::None;
		}
		gluedPlayer = nullptr;
		glueAxis = GlueAxis::None;
		pushText->SetText("Segure E para empurrar/puxar");
	}

	// Morte do Jogador
	if (gluedPlayer && gluedPlayer->IsDead()) {
		isTouching = false;
		gluedPlayer = nullptr;
		return;
	}

	// Comportamento de Seguir o Jogador
	if (gluedPlayer != nullptr) {
		if (glueAxis == GlueAxis::Horizontal) {
			associated.box.y = glueAnchor;
			gluedPlayer->box.y = playerAnchor;
			associated.box.x = gluedPlayer->box.x + blockOffset;
		} else if (glueAxis == GlueAxis::Vertical) {
			associated.box.x = glueAnchor;
			gluedPlayer->box.x = playerAnchor;
			associated.box.y = gluedPlayer->box.y + blockOffset;
		}
	}

	// O SISTEMA DE TRAVA COM PRIORIDADE (CAIXA = NIVEL 1)
    int myPriority = 1; // Caixa tem prioridade baixa
    bool wantsToInteract = (isTouching || gluedPlayer != nullptr);

    if (wantsToInteract) {
        // Pega a trava se: Estiver livre, OU ja for o dono, OU prioridade MAIOR que a do dono atual
        if (GameData::activeInteraction == nullptr ||
            GameData::activeInteraction == this ||
            myPriority > GameData::interactionPriority) {

            GameData::activeInteraction = this;
            GameData::interactionPriority = myPriority;
            pushText->Show();
        } else {
            // Se o dono atual for mais forte do que eu
            pushText->Hide();
        }
    } else {
        // Se eu nao quero mais interagir e era o dono, liberto o microfone E zero a prioridade
        if (GameData::activeInteraction == this) {
            GameData::activeInteraction = nullptr;
            GameData::interactionPriority = 0;
        }
        pushText->Hide();
    }

	isTouching = false;
}

void Pushable::Render() {}


void Pushable::NotifyCollision(GameObject& other) {
InputManager &inputManager = InputManager::GetInstance();

    // COLISAO CONTRA PAREDES E OUTRAS CAIXAS
    if (other.tag == "wall" || other.tag == "pushable") {

        // Se esta caixa NAO esta sendo segurada, ela age como uma parede
        if (gluedPlayer == nullptr) {
            return;
        }

        // Segurada, a fisica afasta a caixa do obstaculo.
        Collision::ResolveOverlap(associated, other);

        // Medimos a direcao fisica da batida (Diferenca entre os centros)
        float dx = associated.box.GetCenter().x - other.box.GetCenter().x;
        float dy = associated.box.GetCenter().y - other.box.GetCenter().y;

        if (glueAxis == GlueAxis::Horizontal) {
            // Sincroniza a posicao do jogador para ser arrastado junto
            gluedPlayer->box.x = associated.box.x - blockOffset;

            // So bloqueia o caminhar do jogador se a batida foi HORIZONTAL
            if (std::abs(dx) > std::abs(dy)) {
                float blockDirX = (dx < 0) ? 1.0f : -1.0f;
                gluedPlayer->SetCollisionNormal(Vec2(blockDirX, 0));
            }
        }
        else if (glueAxis == GlueAxis::Vertical) {
            gluedPlayer->box.y = associated.box.y - blockOffset;

            // So bloqueia o caminhar do jogador se a batida foi VERTICAL
            if (std::abs(dy) > std::abs(dx)) {
                float blockDirY = (dy < 0) ? 1.0f : -1.0f;
                gluedPlayer->SetCollisionNormal(Vec2(0, blockDirY));
            }
        }
        return;
    }

	// INTERACAO COM O JOGADOR
    if (other.tag == "player") {
        isTouching = true;

        Character* charComp = other.GetComponent<Character>();
        if (!charComp) return;

        pushDirection = (associated.box.GetCenter() - other.box.GetCenter()).Normalize();

        // Tenta agarrar a caixa
        if (inputManager.IsKeyDown(E_KEY) && gluedPlayer == nullptr && !charComp->isGlued) {
            GlobalSounds::Button().Play(0);
            gluedPlayer = &other;

            if (std::abs(pushDirection.x) > std::abs(pushDirection.y)) {
                glueAxis = GlueAxis::Horizontal;
                glueAnchor = associated.box.y;
                playerAnchor = other.box.y;
                blockOffset = associated.box.x - other.box.x;
            } else {
                glueAxis = GlueAxis::Vertical;
                glueAnchor = associated.box.x;
                playerAnchor = other.box.x;
                blockOffset = associated.box.y - other.box.y;
            }

            pushText->SetText("Solte E para soltar");
            charComp->isGlued = true;
            charComp->glueAxis = static_cast<Character::GlueAxis>(glueAxis);
        }

        // Se o jogador ja esta segurando ESTA caixa, a fisica nao faz nada
        if (gluedPlayer != nullptr) {
            return;
        }

        // Se o jogador apenas esbarrar na caixa (sem segurar o 'E'), a caixa age como parede
        if (gluedPlayer == nullptr && isTouching) {
            Collision::ResolveOverlap(other, associated);
            return;
        }
    }
}
