#pragma once
#include "Object.h"
class Player : public Object
{
	//private members
private:
	const float rotationSpeed = 0.2f;
	const float speed = 0.005;
	//public members
public:

	//private functions
private:

	//public functions
public:
	float GetSpeed();
	float GetRotationSpeed();
};

