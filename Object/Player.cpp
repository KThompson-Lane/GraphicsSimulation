#include "Player.h"

void Player::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight)
{
	glDisable(GL_CULL_FACE);
	Object::render(viewingMatrix, ProjectionMatrix, showCollider, lights, playerSpotLight);
	glEnable(GL_CULL_FACE);
}

float Player::GetSpeed()
{
	return speed;
}

float Player::GetRotationSpeed()
{
	return rotationSpeed;
}

void Player::AddForce(glm::vec3 force)
{
	velocity += force;
	if (velocity.x + velocity.y + velocity.z > terminalVelocity)
	{
		velocity -= force;
	}
}

void Player::UpdatePosition(float deltaTime)
{
	if (landed && landedObject != nullptr)
	{
		transform->Move(landedObject->transform->position + landingPosition);
	}
	else 
	{
		transform->Move(velocity, deltaTime);
	}
}

void Player::Land(glm::vec3 landingPos, Object& landingObject)
{
	landed = true;
	velocity = glm::vec3(0.0f);
	landedObject = &landingObject;
	landingPosition = landingPos;
}

void Player::TakeOff()
{
	landed = false;
	landedObject = nullptr;
}

void Player::Crash()
{
	this->amount += glm::length(velocity) * 10;
	destroyed = true;
}

void Player::Reset(glm::vec3 resetPosition)
{
	destroyed = false;
	//transform.Reset();
	//transform.Move(resetPosition);
	amount = 0.0f;
	velocity = glm::vec3(0.0f);
	landedObject = nullptr;
	landed = false;
}

bool Player::CheckCollision(Object& other)
{
	if (Object::CheckCollision(other))
	{
		//In collision, need response
		float dist = distance(other.transform->position, this->transform->position);
		glm::vec3 repulseDirection = normalize(transform->position - other.transform->position);

		float P = collider->CalculatePenetration(other.collider, dist);
		transform->Move(repulseDirection, P);

		//This shouldn't be needed but is 
		if (destroyed)
		{
			return false;
		}
		//Check crash conditions
		if (other.tag == "star" || glm::length(velocity) > 0.5f || distance(glm::normalize(transform->Up()), repulseDirection) > 0.5f)
		{
			Crash();
			velocity = glm::vec3(0.0f);
		}
		//Land on body
		Land(repulseDirection * P, other);
		return true;
	}
	else return false;
}

std::pair<float, float> Player::GetMinMaxZoom()
{
	float minZoom = GetColliderSphereRadius() * 1.5;
	float maxZoom = GetColliderSphereRadius() * 15;
	return std::make_pair(minZoom, maxZoom);
}