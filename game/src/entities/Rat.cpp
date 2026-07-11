#include "Rat.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Collider.h"
#include "Game.h"
#include "InputManager.h"
#include "GameData.h"
#include "Quiz.h"
#include "QuizUI.h"
#include "EndState.h"
#include "Log.h"
#include "GlobalSounds.h"

Rat::Rat(GameObject &associated, int x, int y)
    : Component(associated)
{
    associated.tag = "rat";

    SpriteRenderer *spriteRenderer = new SpriteRenderer(associated,
            "game/assets/spritesheets/rat.png", 8, 2);
    Animator *animator = new Animator(associated);
    Collider *collider = new Collider(associated, Vec2(1.25, 1.25), Vec2(0, 0));
    collider->disabled = true;

    spriteRenderer->SetPosition(x, y);
    spriteRenderer->SetScale(2, 2);

    associated.AddComponent(spriteRenderer);
    associated.AddComponent(animator);
    associated.AddComponent(collider);

    animator->AddAnimation("idle", Animation(0, 15, 0.2));
    animator->SetAnimation("idle");

    SDL_Color white = {255, 255, 255, 255};

    GameObject *textGameObject = new GameObject();
    promptText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Aperte E para conversar", white);
    promptText->Hide();
    textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
    textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
    associated.AddComponent(promptText);
}

Rat::~Rat() {}

void Rat::Start() {
    Log::info("RAT - Starting rat component");
}

void Rat::Update(float /*dt*/) {
    if (isTouching) {
        promptText->Show();
    } else {
        promptText->Hide();
    }

    isTouching = false;
    triggered = false;

    if (quizStarted) {
        std::weak_ptr<GameObject> quizObj = Game::GetInstance().GetCurrentState().GetObjectByTag("quiz");
        auto quizGo = quizObj.lock();
        if (!quizGo) {
            quizStarted = false;
            return;
        }
        Quiz *quiz = quizGo->GetComponent<Quiz>();
        if (quiz && quiz->GetState() == QuizProgressState::Finished) {
            Log::info("RAT - Quiz finished, pushing EndState");
            GameData::playerVictory = quiz->IsAllCorrect();
            Game::GetInstance().Push(new EndState());
            quizStarted = false;
        }
    }
}

void Rat::Render() {}

void Rat::NotifyCollision(GameObject &other) {
    if (other.tag != "player" || triggered) return;

    isTouching = true;

    InputManager &inputManager = InputManager::GetInstance();

    if (inputManager.KeyPress(E_KEY) && !GameData::dialogueActive && !quizStarted) {
        GlobalSounds::Button().Play(0);
        Log::info("RAT - Starting quiz");

        triggered = true;
        quizStarted = true;

        std::vector<QuizData> quizData = {
            {
                "Esse santo foi versado em várias áreas da língua e o primeiro a transcrever o Tupi, ajudou a fundar São Paulo e Rio de Janeiro.",
                {"José de Anchieta", "São Francisco", "São José"},
                0
            },
            {
                "Esse missionário foi morto por aqueles a quem ensinava a piedade e a compaixão cristã.",
                {"Judas", "William Carey", "Juan del Castillo"},
                2
            },
            {
                "Este santo brasileiro, cercado de rumores sobrenaturais, ficou conhecido por suas pílulas milagrosas.",
                {"Frei Gilson", "Frei Galvão", "Frei Damião"},
                1
            }
        };

        State &currentState = Game::GetInstance().GetCurrentState();
        GameObject *quizObject = new GameObject();
        QuizUI *quizUI = new QuizUI(*quizObject);
        Quiz *quiz = new Quiz(*quizObject, quizData);
        quizObject->AddComponent(quiz);
        quizObject->AddComponent(quizUI);
        currentState.AddObject(quizObject);
    }
}
