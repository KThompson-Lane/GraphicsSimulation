#include "Player.h"

void Player::UpdateTransform(glm::vec3 incTranslation, glm::vec3 incRotation)
{
	//First rotate object, then translate it.
	objectRotation += incRotation;
	objectPosition += incTranslation;

	objectModelMatrix = glm::rotate(objectModelMatrix, incRotation.x, glm::vec3(1, 0, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, incRotation.y, glm::vec3(0, 1, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, incRotation.z, glm::vec3(0, 0, 1));
	objectModelMatrix = glm::translate(objectModelMatrix, incTranslation);
}

glm::mat4 Player::CameraMount()
{
	return glm::translate(objectModelMatrix, glm::vec3(0.0, -5.0, 3.0));
}
glm::mat4 Player::CameraTarget()
{
	return glm::translate(objectModelMatrix, glm::vec3(0.0, 5.0, 1.0));
}