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
//#include "Light.h"
#include <iostream>

MazeState::MazeState() : State(), //music("game/assets/audio/BGM.wav"),
	loopCount(0)//, lightMap(nullptr)
{
	Log::info("MAZE_STATE - Initializing state");

	Camera::GetInstance().SetPosition(0, 0);
	Camera::GetInstance().SetSpeed(200, 200);

	/*
	// Alocacao do Render Target dedicado para a Iluminacao
	SDL_Renderer* renderer = Game::GetInstance().GetRenderer();
	lightMap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1200, 900); // Ajuste para a sua resolução
	SDL_SetTextureBlendMode(lightMap, SDL_BLENDMODE_MOD);
	*/
}

MazeState::~MazeState()
{
    Log::info("MAZE_STATE - Destroying state");
    objectArray.clear();
	/*
    if (lightMap != nullptr) {
        SDL_DestroyTexture(lightMap);
    }
	*/
}

void MazeState::Start()
{
    StartArray();
}

void MazeState::LoadAssets()
{
    Vec2 tileScale = GameData::tileScale;

    Log::debug("MAZE_STATE - Starting TileMap game object (TMX)");
    GameObject* tileMapGameObject = new GameObject();
    TileSet* tileSet = new TileSet(16, 16, "game/assets/tiles/test_tileset.png");
    // Lendo o labirinto infinito desenhado no Tiled
    TileMap* tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/maze.tmx", tileSet);
    tileMap->scale = tileScale;
    tileMapGameObject->AddComponent(tileMap);
    this->AddObject(tileMapGameObject);

    GameObject* characterGameObject = new GameObject();
    Character* character = new Character(*characterGameObject, "game/assets/img/Player.png");
    character->player = character;

    Collider* collider = new Collider(*characterGameObject, Vec2(1, 1), Vec2(1, 1));
    PlayerController* playerController = new PlayerController(*characterGameObject);
    //Light* heroLight = new Light(*characterGameObject, "game/assets/img/light.png", 3.0f);

    characterGameObject->AddComponent(character);
    characterGameObject->AddComponent(collider);
    characterGameObject->AddComponent(playerController);
    //characterGameObject->AddComponent(heroLight); // A lanterna do labirinto
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

    tileObjects.Load(*this);
    Log::debug("MAZE_STATE - TileObjects loader finished");

    
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
	/*
    SDL_SetRenderTarget(renderer, lightMap);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    // Desenha as luzes na pelicula
    Light::RenderAll();

    // Devolve para a tela principal
	*/
    SDL_SetRenderTarget(renderer, nullptr);

    // Renderiza o jogo
    RenderArray();

    // Aplica a mascara de luz multiplicativa
    //SDL_RenderCopy(renderer, lightMap, nullptr, nullptr);
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
}

int MazeState::GetLoopCount() const { return loopCount; }
