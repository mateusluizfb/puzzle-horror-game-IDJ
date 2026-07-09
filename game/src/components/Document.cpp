#include "Document.h"

#include "DialogueBox.h"
#include "CreateDialogueBox.h"
#include "Game.h"
#include "GameData.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Log.h"
#include "Rect.h"
#include "State.h"
#include "Text.h"
#include "GlobalSounds.h"
#include "Collider.h"

Document::Document(GameObject& associated, const std::string& documentText, DialogueBox::PortraitMode portraitMode)
  : Component(associated)
  , documentText(documentText)
  , portraitMode(portraitMode)
{
  Log::debug("DOCUMENT - Registered for object: " + associated.tag +
             " -> text length: " + std::to_string(documentText.size()));

  GameObject* textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  promptText = new Text(*textGameObject,
                        "game/assets/font/neodgm.ttf",
                        32,
                        Text::BLENDED,
                        "Aperte E para ler",
                        white);
  promptText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(promptText);
}

void Document::Start() {
  Collider *collider = associated.GetComponent<Collider>();
  collider->disabled = true;
}

void Document::Update(float /*dt*/) {
	// O SISTEMA DE TRAVA COM PRIORIDADE (DOCUMENTO = NIVEL 2)
	int myPriority = 2;
	if (isTouching && (GameData::activeInteraction == nullptr ||
				GameData::activeInteraction == this ||
				myPriority > GameData::interactionPriority)) {
		GameData::activeInteraction = this;
		GameData::interactionPriority = myPriority;
		promptText->Show();
	} else {
		if (GameData::activeInteraction == this) {
			GameData::activeInteraction = nullptr;
			GameData::interactionPriority = 0;
		}
		promptText->Hide();
	}
	isTouching = false;

	if (!dialogueObject.expired()) {
		auto go = dialogueObject.lock();
		DialogueBox* db = go->GetComponent<DialogueBox>();
		if (db && db->IsFinished()) {
			Log::info("DOCUMENT - Dialogue finished, closing");
			go->RequestDelete();
			GameData::dialogueActive = false;
			dialogueOpen = false;
			dialogueObject.reset();
		}
	}
}

void Document::Render() {}

void Document::NotifyCollision(GameObject& other) {
  if (other.tag != "player") return;

  isTouching = true;

  InputManager& inputManager = InputManager::GetInstance();

  if (inputManager.KeyPress(E_KEY) && !dialogueOpen && !GameData::dialogueActive) {
    GlobalSounds::Button().Play(0);
    Log::info("DOCUMENT - Opening dialogue");

    SDL_Color white = {255, 255, 255, 255};
    Rect box = Rect(
        0,
        Game::GetInstance().GetWindowHeight() / 1.5,
        Game::GetInstance().GetWindowWidth(),
        Game::GetInstance().GetWindowHeight() / 4);

    State& currentState = Game::GetInstance().GetCurrentState();
    GameObject* go = CreateDialogueBox(
        box,
        "game/assets/font/neodgm.ttf", 24, white,
        documentText,
        {},
        portraitMode);

    currentState.AddObject(go);
    dialogueObject = currentState.GetObjectPtr(go);
    GameData::dialogueActive = true;
    dialogueOpen = true;
  }
}
