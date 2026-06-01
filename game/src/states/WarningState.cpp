#include "WarningState.h"
//#include "MazeState.h"

WarningState::WarningState() {
	// Garante que o gerenciador de recursos aloque as fontes antes do RemakeTexture
	LoadAssets();
	Log::info("StagePush - Transition to stage: WarningState");


	// TITULO DA TELA (Vermelho)
	GameObject* titleObj = new GameObject();
	SDL_Color redColor = {255, 0, 0, 255};
	Text* titleComp = new Text(*titleObj, "game/assets/font/neodgm.ttf", 46,
			Text::BLENDED, "DIRETRIZES DO LABIRINTO", redColor);
	titleObj->AddComponent(titleComp);

	titleObj->box.x = 600.0f - (titleObj->box.w / 2.0f);
	titleObj->box.y = 200.0f;
	AddObject(titleObj);

	// REGRA 1: Paredes Mortais
	GameObject* rule1Obj = new GameObject();
	SDL_Color whiteColor = {255, 255, 255, 255};
	Text* rule1Comp = new Text(*rule1Obj, "game/assets/font/neodgm.ttf", 26, Text::BLENDED, "- NAO ENCOSTE NAS PAREDES DO CORREDOR.", whiteColor);
	rule1Obj->AddComponent(rule1Comp);

	rule1Obj->box.x = 600.0f - (rule1Obj->box.w / 2.0f);
	rule1Obj->box.y = 380.0f;
	AddObject(rule1Obj);

	// REGRA 2: Objetos Mortais
	GameObject* rule2Obj = new GameObject();
	Text* rule2Comp = new Text(*rule2Obj, "game/assets/font/neodgm.ttf", 26, Text::BLENDED, "- NAO TOQUE NOS OBJETOS DEIXADOS NO CHAO.", whiteColor);
	rule2Obj->AddComponent(rule2Comp);

	rule2Obj->box.x = 600.0f - (rule2Obj->box.w / 2.0f);
	rule2Obj->box.y = 460.0f;
	AddObject(rule2Obj);

	// REGRA 3: Direcao Unica (pista falsa)
	GameObject* rule3Obj = new GameObject();
	Text* rule3Comp = new Text(*rule3Obj, "game/assets/font/neodgm.ttf", 26, Text::BLENDED, "- SIGA APENAS EM FRENTE. O RETORNO NAO E OPCAO.", whiteColor);
	rule3Obj->AddComponent(rule3Comp);

	rule3Obj->box.x = 600.0f - (rule3Obj->box.w / 2.0f);
	rule3Obj->box.y = 540.0f;
	AddObject(rule3Obj);

	// PROMPT DE CONFIRMACAO (Texto Cinza de instrucao)
	GameObject* promptObj = new GameObject();
	SDL_Color grayColor = {160, 160, 160, 255};
	Text* promptComp = new Text(*promptObj, "game/assets/font/neodgm.ttf", 22, Text::BLENDED, "Pressione ESPACO para aceitar os termos...", grayColor);
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
	UpdateArray(dt);

	if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
		quitRequested = true;

	// Ao pressionar Espaco, remove este aviso da pilha e empurra o labirinto real
	if (input.KeyPress(SDLK_SPACE)) {
		popRequested = true;
		Game::GetInstance().Push(new MazeState());
	}
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
	//Camera::pos = {0, 0};
}

void WarningState::LoadAssets() {
	Resources::GetFont("game/assets/font/neodgm.ttf", 46);
	Resources::GetFont("game/assets/font/neodgm.ttf", 26);
	Resources::GetFont("game/assets/font/neodgm.ttf", 22);
}
