#pragma once
#include "Object.h"
class Player : public Object
{
	//private members
private:
	const float rotationSpeed = 1;
	const float speed = 0.05;
	glm::vec3 incTranslation;
	//public members
public:

	//private functions
private:

	//public functions
public:
	void Fly(float throttle, glm::vec3 incRotation);
};

