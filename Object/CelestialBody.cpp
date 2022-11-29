#include "CelestialBody.h"

CelestialBody::CelestialBody(int index) 
{
	this->index = index;
	mass = 0.0f;
	radius = 0.0f;
	initialVelocity = glm::vec3(0.0f);
	currentVelocity = glm::vec3(0.0f);
}

void CelestialBody::init(char* modelFile, glm::vec3 position, glm::vec3 rotation, float mass)
{
	Object::init(modelFile);
	objectPosition = position;
	objectRotation = glm::quat(glm::radians(rotation));
	this->mass = mass;
	radius = GetColliderSphereRadius();
	currentVelocity = initialVelocity;
}
void CelestialBody::SetPosition(glm::vec3 position)
{
	objectPosition = position;
}
void CelestialBody::UpdateVelocity(std::vector<CelestialBody>& allBodies, float deltaTime) 
{
	for (int i = 0; i < allBodies.size(); i++)
	{
		if (allBodies[i].index != this->index)
		{
			CelestialBody* otherBody = &allBodies[i];
			float sqrDistance = glm::distance2(otherBody->GetObjectWorldPosition(), this->objectPosition);

			glm::vec3 forceDirection = glm::normalize(otherBody->GetObjectWorldPosition() - this->objectPosition);
			glm::vec3 force = forceDirection * (G * (mass * otherBody->mass / sqrDistance));
			glm::vec3 acceleration = force / mass;
			currentVelocity += acceleration ;
		}
	}
		
}
void CelestialBody::UpdatePosition(float deltaTime)
{
	objectPosition += currentVelocity ;
}