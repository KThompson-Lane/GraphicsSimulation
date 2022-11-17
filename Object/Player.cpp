#include "Player.h"

void Player::Fly(float throttle, glm::vec3 incRotation)
{
	incRotation *= rotationSpeed;
	objectRotation += incRotation;
	if (objectRotation.x >= 360.0f)
	{
		objectRotation.x -= 360.0f;
	}
	else if(objectRotation.x < 0.0f)
	{
		objectRotation.x += 360.0f;
	}

	if (objectRotation.x >= 360.0f)
	{
		objectRotation.x -= 360.0f;
	}
	else if (objectRotation.x < 0.0f)
	{
		objectRotation.x += 360.0f;
	}

		if (objectRotation.x >= 360.0f)
	{
		objectRotation.x -= 360.0f;
	}
	else if(objectRotation.x < 0.0f)
	{
		objectRotation.x += 360.0f;
	}

	//moves it along the up direction
	float movementAmount = throttle * speed;
	objectPosition += Up() * movementAmount;;
}