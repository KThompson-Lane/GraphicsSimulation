#include "Player.h"

float Player::GetSpeed()
{
	return speed;
}

float Player::GetRotationSpeed()
{
	return rotationSpeed;
}

glm::vec3 Player::GetVelocity()
{
	return currentVelocity;
}

void Player::AddForce(glm::vec3 force)
{
	this->currentVelocity += force;
}
void Player::UpdatePosition(float deltaTime)
{
	objectPosition += currentVelocity;
}