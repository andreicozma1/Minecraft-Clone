#pragma once
#include <math.h>
#include <Biomes.h>
#include <Game.h>
class Player
{
public:
	Player();

	~Player();

	void Jump(float height, float speed);
	void Main(Game g);

	Biomes biomes;


	struct {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} position;
	struct {
		float x = 0;
		float y = 0;
		float z = 0;
	} looking;
	struct {
		int health = 5;
		int hunger = 0;
	} vitals;

	struct {
		float forward = 0.15f;
		float fly = .5f;
		float fall = -0.25f;
		float strafe = 0.1f;
		float runMultiplier = 15;

	} speed;
	float height = 1.2f;
	bool shouldJump = false;
	bool canFly = true;
	float jumpCounter = 0;
	float jumpHeight = height * 2;
	float jumpSpeed = 6;
	int jumpBlockX = 0;
	int jumpBlockZ = 0;

	
};

