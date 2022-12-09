#include "Craft.h"


void Craft::CollideWithPlayer(glm::vec3 impactVelocity)
{
	inOrbit = false;
	glm::vec3 newVelocity = (float) (glm::length(impactVelocity) * 0.1) * glm::normalize(impactVelocity);
	this->amount = pow(glm::length(impactVelocity), 3);
	this->velocity = newVelocity;
}

void Craft::UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime)
{
	if (inOrbit)
	{
		CelestialBody::UpdateOrbit(orbitBodyPosition, deltaTime);
	}
	else
	{
		transform->Move(velocity, deltaTime);
	}
}