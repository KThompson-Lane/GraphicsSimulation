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
		objectPosition = landedObject->GetObjectWorldPosition() + landingPosition;
	}
	else 
	{
		objectPosition += velocity * deltaTime;
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