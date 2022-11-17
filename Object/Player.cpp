#include "Player.h"

void Player::Fly(float throttle, glm::vec3 incRotation)
{
	float yawInput = incRotation.x * rotationSpeed;
	float pitchInput = incRotation.y * rotationSpeed;
	float RollInput = incRotation.z * rotationSpeed;

	glm::quat yaw = glm::angleAxis(yawInput, Side());
	glm::quat pitch = glm::angleAxis(pitchInput, Up());
	glm::quat roll = glm::angleAxis(RollInput, Forward());
	
	objectRotation = (yaw * pitch * roll * objectRotation);

	//moves it along the up direction
	objectPosition += Up() * throttle * speed;;
}