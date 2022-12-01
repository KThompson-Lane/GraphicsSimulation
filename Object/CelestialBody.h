#pragma once
#include "Object.h";

class CelestialBody : public Object
{
	//private members
private:
	//public members
	const float G = 0.0000694f;
public:
	int orbitingBodyIndex = -1;
	int index;
	float radius;
	float orbitalSpeed;
	float orbitDistance;
	float orbitAmount = 0.0f;
	//private functions
private:

	//public functions
public:
	CelestialBody(int index);
	void init(char* modelFile, glm::vec3 initialPosition, glm::vec3 initialRotation);
	void SetPosition(glm::vec3 position);
	void SetOrbit(int orbitingBodyIndex, float speed, float distance);
	void UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime);
};