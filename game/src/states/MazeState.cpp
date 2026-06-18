#include "MazeState.h"
#include "Log.h"
#include "TileSet.h"
#include "TileMap.h"
#include "InputManager.h"
#include "Character.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Camera.h"
#include "EndState.h"
#include "Game.h"
#include "TileObjects.h"
#include "TileObject.h"
#include "Wall.h"
#include "Teleporter.h"
#include "Light.h"
#include <iostream>

MazeState::MazeState() : State(), //music("game/assets/audio/BGM.wav"),
	loopCount(0), lightMap(nullptr)
{
	Log::info("MAZE_STATE - Initializing state");

	Camera::GetInstance().SetPosition(0, 0);
	Camera::GetInstance().SetSpeed(200, 200);

	// Alocacao do Render Target dedicado para a Iluminacao
	SDL_Renderer* renderer = Game::GetInstance().GetRenderer();
	lightMap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1200, 900);
	SDL_SetTextureBlendMode(lightMap, SDL_BLENDMODE_MOD);
}

MazeState::~MazeState()
{
    Log::info("MAZE_STATE - Destroying state");
    objectArray.clear();
    if (lightMap != nullptr) SDL_DestroyTexture(lightMap);
}

void MazeState::Start()
{
    StartArray();
}

void MazeState::LoadAssets()
{
    Vec2 tileScale = GameData::tileScaleFar;

    Log::debug("MAZE_STATE - Starting TileMap game object (TMX)");
    GameObject* tileMapGameObject = new GameObject();
    TileSet* tileSet = new TileSet(16, 16, "game/assets/tiles/test_tileset.png");
    // Lendo o labirinto infinito desenhado no Tiled
    TileMap* tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/maze.tmx", tileSet);
    tileMap->scale = tileScale;
    tileMapGameObject->AddComponent(tileMap);
    this->AddObject(tileMapGameObject);

    GameObject* characterGameObject = new GameObject();
    Character* character = new Character(*characterGameObject,
			"game/assets/img/Player_Small.png");
    character->player = character;

    Collider* collider = new Collider(*characterGameObject, Vec2(1, 1), Vec2(1, 1));
    PlayerController* playerController = new PlayerController(*characterGameObject);
	Light* heroLight = new Light(*characterGameObject, "game/assets/img/light.png", 0.2f, 255, 255, 255, 200);

    characterGameObject->AddComponent(character);
    characterGameObject->AddComponent(collider);
    characterGameObject->AddComponent(playerController);
	characterGameObject->AddComponent(heroLight);
    characterGameObject->tag = "player";

    this->AddObject(characterGameObject);

    SpriteRenderer* spriteRenderer1 = characterGameObject->GetComponent<SpriteRenderer>();

    // Ajuste a posicao de spawn para o inicio do Corredor A
    spriteRenderer1->SetPosition(800, 600);

    Log::debug("MAZE_STATE - Starting TileObjects loader");
    TileObjects tileObjects(
        "game/assets/tiles/maze.tmx",
        "game/assets/tiles/test_tileset.png",
        tileScale
    );

    // Registrando a Fabrica de Componentes do Labirinto
    tileObjects.RegisterComponent("wall", [](GameObject& go) -> Component* {
        return new Wall(go);
    });
    tileObjects.RegisterComponent("collider", [](GameObject& go) -> Component* {
        return new Collider(go, Vec2(1, 1), Vec2(0, 0));
    });

	// TELETRANSPORTE
    tileObjects.RegisterComponent("teleporter", [](GameObject& go) -> Component* {
        const TileObjectData& data = go.GetComponent<TileObject>()->GetData();

        float dest_x = 0;
        float dest_y = 0;
        bool completes_loop = false;

        if (data.properties.find("destX") != data.properties.end()) dest_x = std::stof(data.properties.at("destX"));
        if (data.properties.find("destY") != data.properties.end()) dest_y = std::stof(data.properties.at("destY"));
        if (data.properties.find("completeLoop") != data.properties.end()) completes_loop = (data.properties.at("completeLoop") == "true");

        return new Teleporter(go, Vec2(dest_x, dest_y), completes_loop);
    });

	// PORTAS
	tileObjects.RegisterComponent("door", [](GameObject& go) -> Component* {
        const TileObjectData& data = go.GetComponent<TileObject>()->GetData();

        float dest_x = 0;
        float dest_y = 0;
		float fail_x = 0;
        float fail_y = 0;
        std::string side = "none";
        bool isWin = false;
        bool isLose = false;

        // Propriedades desenhadas no Tiled
        try {
            if (data.properties.find("destX") != data.properties.end()) dest_x = std::stof(data.properties.at("destX"));
            if (data.properties.find("destY") != data.properties.end()) dest_y = std::stof(data.properties.at("destY"));
			if (data.properties.find("failX") != data.properties.end()) fail_x = std::stof(data.properties.at("failX"));
            if (data.properties.find("failY") != data.properties.end()) fail_y = std::stof(data.properties.at("failY"));
            if (data.properties.find("side") != data.properties.end()) side = data.properties.at("side");

            // As flags de clímax (se não existirem no Tiled, assumem falso)
            if (data.properties.find("isWin") != data.properties.end()) isWin = (data.properties.at("isWin") == "true");
            if (data.properties.find("isLose") != data.properties.end()) isLose = (data.properties.at("isLose") == "true");
        } catch (const std::exception& e) {
            Log::warning("[TILED] Propriedades ausentes na porta. Usando fallback.");
        }

		return new Door(go, Vec2(dest_x, dest_y), Vec2(fail_x, fail_y), side, isWin, isLose);
    });

	// LUZES
	tileObjects.RegisterComponent("light", [](GameObject& go) -> Component* {
		const TileObjectData& data = go.GetComponent<TileObject>()->GetData();

		// Valores padrao (Luz branca forte)
		float scale = 3.0f;
		Uint8 r = 255, g = 255, b = 255, a = 255;

		if (data.properties.find("scale") != data.properties.end()) scale = std::stof(data.properties.at("scale"));
		if (data.properties.find("r") != data.properties.end()) r = (Uint8)std::stoi(data.properties.at("r"));
		if (data.properties.find("g") != data.properties.end()) g = (Uint8)std::stoi(data.properties.at("g"));
		if (data.properties.find("b") != data.properties.end()) b = (Uint8)std::stoi(data.properties.at("b"));
		if (data.properties.find("a") != data.properties.end()) a = (Uint8)std::stoi(data.properties.at("a"));

		return new Light(go, "game/assets/img/light.png", scale, r, g, b, a);
	});

	// DARKZONE
	tileObjects.RegisterComponent("darkZone", [](GameObject& go) -> Component* {
        return new DarkZone(go);
    });

	// SANTOS
	tileObjects.RegisterComponent("saint", [](GameObject& go) -> Component* {
		return new Saint(go);
	});

    tileObjects.Load(*this);
    Log::debug("MAZE_STATE - TileObjects loader finished");

    Camera::GetInstance().Follow(characterGameObject);

	srand(time(NULL));
    ShuffleDoors();
}

void MazeState::Update(float dt) {
    InputManager& inputManager = InputManager::GetInstance();

    std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
    if (playerPtr.expired())
    {
        Log::info("MAZE_STATE - Player is dead, switching to EndState");
        music.Stop();
        popRequested = true;
        GameData::playerVictory = false;
        Game::GetInstance().Push(new EndState());
    }

    if (inputManager.QuitRequested())
    {
        Log::warning("MAZE_STATE - Quit requested via SDL event");
        music.Stop();
        this->RequestQuit();
    }

    if (inputManager.KeyPress(ESCAPE_KEY))
    {
        Log::info("MAZE_STATE - Escape key pressed, popping state");
        music.Stop();
        this->RequestPop();
    }

	// Atualiza movimento e posicoes
    UpdateArray(dt);

    // Colisoes baseadas nas posicoes atualizadas
    collisionSystem.Update(objectArray);

    // Remove os objetos destruidos
    UpdateDead();

    Camera::GetInstance().Update(dt);
}

void MazeState::Render() {
    SDL_Renderer* renderer = Game::GetInstance().GetRenderer();

    // Redireciona a pintura para a pelicula escura
    SDL_SetRenderTarget(renderer, lightMap);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    // Desenha as luzes na pelicula
    Light::RenderAll();

    // Devolve para a tela principal
    SDL_SetRenderTarget(renderer, nullptr);
    RenderArray();
    // Aplica a mascara de luz multiplicativa
    SDL_RenderCopy(renderer, lightMap, nullptr, nullptr);
}

void MazeState::Pause() {
    Log::info("MAZE_STATE - Pausing state");
}

void MazeState::Resume() {
    Log::info("MAZE_STATE - Resuming state");

    // Devolve o foco da camera para o heroi ao voltar para o labirinto
    std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
    if (!playerPtr.expired())
        Camera::GetInstance().Follow(playerPtr.lock().get());
}

void MazeState::AddLoop() {
    loopCount++;
    Log::info("MAZE_STATE - FATOR SANIDADE. Loop concluído: " + std::to_string(loopCount));
	ShuffleDoors();
}

void MazeState::ResetLoop() {
    loopCount = 0;
    Log::info("MAZE_STATE - LOOP RESETADO AO ZERO.");
    ShuffleDoors();
}

void MazeState::ShuffleDoors() {
	Log::info("EMBARALHANDO AS PORTAS (MAIORIA VENCE)...");

	std::vector<std::string> possibleSides;
	std::vector<Door*> roomDoors;
	std::vector<GameObject*> saintsList;

	// LEITOR DINAMICO: Varre o mapa para ver quais portas existem nesta sala
	for (size_t i = 0; i < objectArray.size(); i++) {
		GameObject* go = objectArray[i].get();

		// Guarda as portas do enigma e descobre os lados possiveis
		Door* door = go->GetComponent<Door>();
		if (door != nullptr && !door->isWinCondition && !door->isDeathTrap) {
			roomDoors.push_back(door);
			possibleSides.push_back(door->side); // Salva se eh up, down, right, left, etc.
		}
		// Guarda os Santos
		if (go->tag == "saint") saintsList.push_back(go);
	}

	// Se a sala nao tiver portas (ex: no menu principal), sai da funcao para nao dar crash
	if (possibleSides.empty()) return;

	// Sorteia a direcao correta SOMENTE entre as portas que existem
	std::string correctSide = possibleSides[rand() % possibleSides.size()];

	// Atualiza as portas
	for (Door* door : roomDoors)
		door->isCorrect = (door->side == correctSide);

	// A Matematica da Maioria para os Santos
	int totalSaints = saintsList.size();
	if (totalSaints > 0) {

		// Embaralha a ordem dos santos para que a resposta nao fique sempre nos primeiros
		for (int i = totalSaints - 1; i > 0; --i) {
			int j = rand() % (i + 1);
			std::swap(saintsList[i], saintsList[j]);
		}

		int majority = (totalSaints / 2) + 1;

		for (int i = 0; i < totalSaints; i++) {
			std::string assignedSide;

			// Os primeiros X santos apontam para a resposta certa
			if (i < majority) assignedSide = correctSide;
			// Os demais apontam para mentiras sorteadas das opcoes disponiveis
			else {
				do {
					assignedSide = possibleSides[rand() % possibleSides.size()];
				} while (assignedSide == correctSide);
			}

			// GIRA O GAMEOBJECT DIRETAMENTE (Sem depender de SpriteRenderer!)
			saintsList[i]->angleDeg = 0; // O padrao (olhando para a direita)

			if (assignedSide == "up") saintsList[i]->angleDeg = -90;
			else if (assignedSide == "down") saintsList[i]->angleDeg = 90;
			else if (assignedSide == "left") saintsList[i]->angleDeg = 180;
            // Se for "right", o angulo continua 0.

            // ALARME DE TESTE NO TERMINAL:
            Log::info("[SANTOS] Santo [" + std::to_string(i) + "] foi forçado a olhar para: " + assignedSide);
		}

		Log::info("Sorteio feito! Lado correto: " + correctSide + ". " + std::to_string(majority) + " santos apontam para ele.");
	}
}

int MazeState::GetLoopCount() const { return loopCount; }
