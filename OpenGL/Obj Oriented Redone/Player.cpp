#include "Player.h"


Player::Player()
{
}


Player::~Player()
{
}


void Player::Jump(float height, float speed) {
	if (!shouldJump) {
		jumpHeight = height;
		jumpSpeed = speed;
		shouldJump = true;
		jumpBlockX = position.x;
		jumpBlockZ = position.z;
		jumpCounter = 0;
	}
}

void Player::Main(Game g) {
	if (!canFly) {
		if (!shouldJump) {
			if (position.y > biomes.getHeight(biomes.current, position.x, position.z) + height) {
				position.y += speed.fall;
				speed.fall -= 0.02f * g.frameTimeMultiplier;
			}
			else if (position.y <= biomes.getHeight(biomes.current, position.x, position.z) + height - 0.25f) {
				Jump(height * .7, 9);
			}
			else {
				position.y = biomes.getHeight(biomes.current, position.x, position.z) + height;
				speed.fall = -0.25f;
			}
		}
		else {
			jumpCounter += g.frameTimeMultiplier;
			position.y = -pow(jumpCounter / 80 * jumpSpeed - sqrt(jumpHeight), 2) + jumpHeight + biomes.getHeight(biomes.current, jumpBlockX, jumpBlockZ) + height;
			
			float terrHeight = biomes.getHeight(biomes.current, position.x, position.z);
			if (position.y <= terrHeight + height) {
				position.y = terrHeight + height;
				shouldJump = false;
				jumpCounter = 0;
			}
		}
	}
}