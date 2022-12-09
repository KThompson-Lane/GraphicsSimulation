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
	velocity = glm::vec3(0.0f);
	//First make sure we're on the correct position on the body
	transform->Move(landedObject->transform->position + landingPosition);
	//Then move up slightly to prevent any collision mishaps
	transform->Move(transform->Up(), 0.1f);
	//Finally add a small force in the up direction vector
	AddForce(transform->Up() * 0.00008f);
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
	transform->Reset();
	transform->Move(resetPosition);
	destroyed = false;
	amount = 0.0f;
	velocity = glm::vec3(0.0f);
	landedObject = nullptr;
	landed = false;
}

bool Player::CheckCollision(Object& other)
{
	if (Object::CheckCollision(other))
	{
		//Old code, keep in case needed
		glm::vec3 direction = normalize(transform->position - other.transform->position);

		glm::vec3 translation = collider->CalculatePenetration(other.collider);
		transform->Move(translation);

		//Calculate landing position relative to planet centre, then check crash conditions
		glm::vec3 landingPosition = translation - other.transform->position;

		//Crash conditions:
			//Collided object is star
			//Player velocity is too high
			//Landing angle is not optimal
		if (other.tag == "satellite")
		{
			Crash();
			velocity = glm::normalize(landingPosition) * (float)(glm::length(velocity) / pow(other.GetMass(), 6));
			return true;
		}
		else
		{
			if (other.tag == "star" || glm::length(velocity) > 0.35f || distance(transform->Up(), glm::normalize(landingPosition)) > 0.3f)
			{
				Crash();
				velocity = glm::vec3(0.0f);
			}

			//Land on body
			Land(landingPosition, other);
			return true;
		}
	}
	else return false;
}

std::pair<float, float> Player::GetMinMaxZoom()
{
	float radius = model.CalcBoundingSphere();
	float minZoom = radius * 1.5;
	float maxZoom = radius * 15;
	return std::make_pair(minZoom, maxZoom);
}