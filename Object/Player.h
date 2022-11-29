#pragma once
#include "Object.h"
class Player : public Object
{
	//private members
private:
	const float rotationSpeed = 0.002f;
	const float speed = 0.005;
	glm::vec3 currentVelocity;
	//public members
public:

	//private functions
private:

	//public functions
public:
	float GetSpeed();
	float GetRotationSpeed();
	glm::vec3 GetVelocity();
	void AddForce(glm::vec3 force);
	void UpdatePosition(float deltaTime);
};

