#include "CelestialBody.h"

CelestialBody::CelestialBody(int index)
{
	this->index = index;
}

void CelestialBody::init(char* modelFile, glm::vec3 position, glm::vec3 rotation)
{
	Object::init(modelFile);
	objectPosition = position;
	objectRotation = glm::quat(glm::radians(rotation));
	radius = GetColliderSphereRadius();
}
void CelestialBody::SetPosition(glm::vec3 position)
{
	objectPosition = position;
}

void CelestialBody::SetOrbit(int orbitingBodyIndex, float speed, float distance)
{
	this->orbitingBodyIndex = orbitingBodyIndex;
	orbitalSpeed = speed;
	orbitDistance = distance;
}

void CelestialBody::UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime)
{
	orbitAmount += (orbitalSpeed * deltaTime);
	if (orbitAmount >= 360.0f)
	{
		orbitAmount -= 360.0f;
	}
	else if (orbitAmount < 0.0f)
	{
		orbitAmount += 360.0f;
	}

	glm::mat4 orbitMatrix = glm::translate(glm::mat4(1.0), orbitBodyPosition);
	orbitMatrix = glm::rotate(orbitMatrix, glm::radians(orbitAmount), glm::vec3(0.0, 1.0, 0.0));
	orbitMatrix = glm::translate(orbitMatrix, glm::vec3(orbitDistance, 0.0, 0.0));
	objectPosition.x = orbitMatrix[3][0];
	objectPosition.y = orbitMatrix[3][1];
	objectPosition.z = orbitMatrix[3][2];
}