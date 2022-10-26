#pragma once
#include "Object.h"
class Player : public Object
{
	//private members
private:
	const float rotationSpeed = 0.01;
	const float speed = 0.05;
	//public members
public:

	//private functions
private:

	//public functions
public:
	void Move(float throttle, glm::vec3 incRotation);
	glm::mat4 CameraMount();
	glm::mat4 CameraTarget();
};

