#include "WarningState.h"

WarningState::WarningState() {
	// Garante que o gerenciador de recursos aloque as fontes antes do RemakeTexture
	LoadAssets();
	Log::info("StagePush - Transition to stage: WarningState");


	// TITULO DA TELA (Vermelho)
	GameObject* titleObj = new GameObject();
	SDL_Color redColor = {255, 0, 0, 255};
	Text* titleComp = new Text(*titleObj, "game/assets/font/neodgm.ttf", 46,
			Text::BLENDED, "AVISOS", redColor);
	titleObj->AddComponent(titleComp);

	titleObj->box.x = 600.0f - (titleObj->box.w / 2.0f);
	titleObj->box.y = 200.0f;
	AddObject(titleObj);

	// MISTERIO 1: Os Santos
	GameObject* rule1Obj = new GameObject();
	SDL_Color whiteColor = {255, 255, 255, 255};
	Text* rule1Comp = new Text(*rule1Obj, "game/assets/font/neodgm.ttf", 26,
			Text::BLENDED, "Quando não souber para onde ir os santos conhecem e te guiam ao caminho.", whiteColor);
	rule1Obj->AddComponent(rule1Comp);

	rule1Obj->box.x = 600.0f - (rule1Obj->box.w / 2.0f);
	rule1Obj->box.y = 340.0f;
	AddObject(rule1Obj);

	GameObject* rule11Obj = new GameObject();
	Text* rule11Comp = new Text(*rule11Obj, "game/assets/font/neodgm.ttf", 26,
			Text::BLENDED, "Mas a loucura corrompeu a mente de alguns.", redColor);
	rule11Obj->AddComponent(rule11Comp);

	rule11Obj->box.x = 600.0f - (rule11Obj->box.w / 2.0f);
	rule11Obj->box.y = 380.0f;
	AddObject(rule11Obj);

	// MISTERIO 2: A Luz Branca
	GameObject* rule2Obj = new GameObject();
	Text* rule2Comp = new Text(*rule2Obj, "game/assets/font/neodgm.ttf", 26,
			Text::BLENDED, "Lembre-se: Jamais caminhe pelo rubro...", whiteColor);
	rule2Obj->AddComponent(rule2Comp);

	rule2Obj->box.x = 600.0f - (rule2Obj->box.w / 2.0f);
	rule2Obj->box.y = 460.0f;
	AddObject(rule2Obj);

	// MISTERIO 3: O Vermelho
	GameObject* rule3Obj = new GameObject();
	Text* rule3Comp = new Text(*rule3Obj, "game/assets/font/neodgm.ttf", 26, Text::BLENDED, "Ele anseia por voce.", redColor);
	rule3Obj->AddComponent(rule3Comp);

	rule3Obj->box.x = 600.0f - (rule3Obj->box.w / 2.0f);
	rule3Obj->box.y = 500.0f;
	AddObject(rule3Obj);

	// PROMPT DE CONFIRMACAO (Texto Cinza de instrucao)
	GameObject* promptObj = new GameObject();
	SDL_Color grayColor = {160, 160, 160, 255};
	Text* promptComp = new Text(*promptObj, "game/assets/font/neodgm.ttf", 22, Text::BLENDED, "Pressione ESPACO para aceitar seu destino...", grayColor);
	promptObj->AddComponent(promptComp);

	promptObj->box.x = 600.0f - (promptObj->box.w / 2.0f);
	promptObj->box.y = 750.0f;
	AddObject(promptObj);
}

WarningState::~WarningState() {
	objectArray.clear();
}

void WarningState::Update(float dt) {
	InputManager& input = InputManager::GetInstance();
	std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");

	if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
		quitRequested = true;

	// Ao pressionar Espaco, remove este aviso da pilha e empurra o labirinto real
	if (input.KeyPress(SDLK_SPACE))
		Game::GetInstance().Push(new MazeState());

	if (input.KeyPress(Z_KEY)) {
		Log::info("WARNING_STATE - Z key pressed, popping state");
		//music.Stop();
		this->RequestPop();
	}

	if (input.KeyPress(X_KEY)) {
		Log::info("WARNING_STATE - X key pressed, pushing MazeState");
		//music.Stop();
		Game::GetInstance().Push(new MazeState());
	}


	UpdateArray(dt);
}

void WarningState::Render() {
	// Forca a limpeza de tela com Preto Absoluto para criar isolamento visual
	SDL_Renderer* renderer = Game::GetInstance().GetRenderer();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	RenderArray();
}

void WarningState::Start() {
	StartArray();
}

void WarningState::Pause() {}
void WarningState::Resume() {
	Log::info("WARNING_STATE - Resuming state");
	Camera::GetInstance().Unfollow();
	Camera::GetInstance().SetPosition(0, 0);
}

void WarningState::LoadAssets() {
	Resources::GetFont("game/assets/font/neodgm.ttf", 46);
	Resources::GetFont("game/assets/font/neodgm.ttf", 26);
	Resources::GetFont("game/assets/font/neodgm.ttf", 22);
}
