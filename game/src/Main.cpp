#include "Game.h"
#include "StageState.h"
#include "TitleState.h"
#include "Log.h"

int main(int argc, char **argv)
{
  Game &game = Game::GetInstance("Eu só queria água", 1200, 900);  
  game.Push(new TitleState());

  game.Run();

  delete &game;

  return 0;
}


