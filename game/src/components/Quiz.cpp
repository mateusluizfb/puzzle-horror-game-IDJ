#include "Game.h"
#include "GameData.h"
#include "Log.h"
#include "Quiz.h"
#include "CreateDialogueBox.h"
#include "InputManager.h"
#include "DialogueBox.h"

Quiz::Quiz(GameObject &associated, const std::vector<QuizData>& quizData)
  : Component(associated), quizData(quizData), currentQuestionIndex(0)
  {}

void Quiz::Update(float dt) {
  if (!dialogueObject.expired())
  {
    auto go = dialogueObject.lock();
    DialogueBox *db = go->GetComponent<DialogueBox>();
    if (db && db->IsFinished())
    {
      Log::info("STAGE_STATE - Dialogue finished, removing dialogue box");
      go->RequestDelete();
      GameData::dialogueActive = false;
    }
  }
}

void Quiz::Render() {
}

void Quiz::NotifyCollision(GameObject &other) {
  if (other.tag != "player") return;

  InputManager &inputManager = InputManager::GetInstance();
  Game &game = Game::GetInstance();
  State &currentState = game.GetCurrentState();

  if (inputManager.KeyPress(SDLK_e)) {
    Log::info("QUIZ - Player collided and pressed E, starting quiz");

    if (!GameData::dialogueActive)
    {
      SDL_Color white = {255, 255, 255, 255};
      Rect box = Rect(
          0,
          Game::GetInstance().GetWindowHeight() / 1.5,
          Game::GetInstance().GetWindowWidth(),
          Game::GetInstance().GetWindowHeight() / 4);
      auto go = CreateDialogueBox(
          box,
          "game/assets/font/neodgm.ttf", 24, white,
          "You enter a dark room. The air is cold and stale. "
          "You hear faint whispers coming from the darkness ahead. "
          "What do you do?",
          {"Explore left", "Explore right", "Leave immediately"});
      currentState.AddObject(go);
      dialogueObject = currentState.GetObjectPtr(go);
      GameData::dialogueActive = true;
    }
  }
}
