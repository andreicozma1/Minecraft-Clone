#pragma once
#include <Player.h>

class Game
{
public:
	Game();
	~Game();

	void Run(Player p);

	float frameTimeMultiplier;
};

