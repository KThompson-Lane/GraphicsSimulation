#pragma once
#include "Object.h"
class Player : public Object
{
	//private members
private:
	const float rotationSpeed = 0.02f;
	const float speed = 0.05;
	//public members
public:

	//private functions
private:

	//public functions
public:
	void Fly(float throttle, glm::vec3 incRotation);
};

