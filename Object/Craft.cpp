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
	else if (landed && landedBody != nullptr)
	{
		transform->Move(landedBody->transform->position + landingPosition);
	}
	else
	{
		transform->Move(velocity, deltaTime);
	}
}
bool Craft::CheckCollision(Object& other)
{
	if (Object::CheckCollision(other))
	{
		if (other.tag == "player")
		{
			return true;
		}

		glm::vec3 translation = collider->CalculatePenetration(other.collider);
		transform->Move(translation);

		//Calculate landing position relative to planet centre, then check crash conditions
		glm::vec3 landingPos = translation - other.transform->position;
		landed = true;
		inOrbit = false;
		velocity = glm::vec3(0.0f);
		landedBody = &other;
		landingPosition = landingPos;
	}
}
void Craft::AddForce(glm::vec3 force)
{
	velocity += force * 0.1f;
}