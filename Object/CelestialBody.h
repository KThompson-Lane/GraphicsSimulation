#pragma once
#include "Object.h";

class CelestialBody : public Object
{
	//private members
private:
	//public members
	int index;
	const float G = 0.000694f;
public:
	CelestialBody* orbitingBody = nullptr;
	float mass;
	float radius;
	glm::vec3 initialVelocity;
	glm::vec3 currentVelocity;
	//private functions
private:

	//public functions
public:
	CelestialBody(int index);
	void init(char* modelFile, glm::vec3 initialPosition, glm::vec3 initialRotation, float mass);
	void SetPosition(glm::vec3 position);
	void UpdateVelocity(std::vector<CelestialBody>& allBodies, float deltaTime);
	void UpdatePosition(float deltaTime);
};