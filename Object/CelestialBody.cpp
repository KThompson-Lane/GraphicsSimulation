#include "CelestialBody.h"

void CelestialBody::init(char* modelFile, glm::vec3 position, glm::vec3 rotation)
{
	Object::init(modelFile);
	transform->Move(position);
	transform->Rotate(rotation.x, rotation.y, rotation.z);
	radius = GetColliderSphereRadius();
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
	transform->Move(glm::vec3(orbitMatrix[3][0], orbitMatrix[3][1], orbitMatrix[3][2]));
}
std::pair<float, float> CelestialBody::GetMinMaxZoom()
{
	float minZoom = GetColliderSphereRadius() * 5;
	float maxZoom = GetColliderSphereRadius() * 30;
	return std::make_pair(minZoom, maxZoom);
}