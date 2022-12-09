#pragma once
#include "Object.h";

class CelestialBody : public Object
{
	//private members
private:
	glm::vec3 initialPosition;
	float radius;
	//public members
public:
	int orbitingBodyIndex = -1;
	float orbitalSpeed;
	float orbitAmount = 0.0f;
	//private functions
private:

	//public functions
public:
	CelestialBody(std::string tag) : Object(tag) {}
	void init(char* modelFile, glm::vec3 initialPosition, glm::vec3 initialRotation);
	void SetOrbit(int orbitingBodyIndex, float speed);
	void UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime);
	std::pair<float, float> GetMinMaxZoom();
};