#pragma once
#include "Object.h"

class Player : public Object
{
	//private members
private:
	const float rotationSpeed = 0.2f;
	const float speed = 0.00005;
	const float terminalVelocity = 3.0f;
	glm::vec3 velocity;

	Object* landedObject = nullptr;
	glm::vec3 landingPosition;
	//public members
public:
	bool landed = false;
	//private functions
private:

	//public functions
public:
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight);
	float GetSpeed();
	float GetRotationSpeed();
	void AddForce(glm::vec3 force);
	void UpdatePosition(float deltaTime);
	void Land(glm::vec3 landingPosition, Object& landingObject);
	void TakeOff();
	void Crash();
	void Reset(glm::vec3 resetPosition);
	bool CheckCollision(Object& other);
	glm::vec3 GetVelocity() const { return velocity; }
};

