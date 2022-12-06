#include "Player.h"

void Player::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight)
{
	glDisable(GL_CULL_FACE);
	Object::render(viewingMatrix, ProjectionMatrix, showCollider, lights, playerSpotLight);
	glEnable(GL_CULL_FACE);
	collider.DrawCollider(boundingShader);
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

void Player::Crash()
{
	this->amount += glm::length(velocity) * 10;
}

void Player::Reset(glm::vec3 resetPosition)
{
	objectPosition = resetPosition;
	objectRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
	amount = 0.0f;
	velocity = glm::vec3(0.0f);
	landedObject = nullptr;
	landed = false;
}

void Player::CreateCollider()
{
	collider = SphereCollider(this->model.CalcBoundingSphere(), glm::vec3(0.0, 0.0, 0.0));
	collider.CreateGeometry(this->boundingShader);
}