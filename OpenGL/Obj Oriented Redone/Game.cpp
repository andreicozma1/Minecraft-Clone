#include "Game.h"
#include <Player.h>
Game::Game()
{
}


Game::~Game()
{
}

void Game::Run(Player playerr) {
	playerr.Main(*this);
}
