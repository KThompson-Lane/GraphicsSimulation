#include "CelestialBody.h"

void CelestialBody::init(char* modelFile, glm::vec3 position, glm::vec3 rotation, float gravity)
{
	Object::init(modelFile);
	objectPosition = position;
	objectRotation = glm::quat(glm::radians(rotation));
	gravitationalPull = GetColliderSphereRadius() * gravity;
}

void CelestialBody::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights)
{
	Object::render(viewingMatrix, ProjectionMatrix, showCollider, lights);
}

void CelestialBody::SetOrbit(CelestialBody* body, float speed, float distance)
{
	orbitingBody = body;
	orbitalSpeed = speed;
	orbitDistance = distance;
	objectPosition += glm::vec3(0.0,0.0,1.0) * distance;
}

float CelestialBody::GetGravityDistance()
{
	return gravitationalPull;
}

void CelestialBody::SetPosition(glm::vec3 position)
{
	this->objectPosition = position;
}
