#pragma once
#include "Object.h";

class CelestialBody : public Object
{
	using Object::Object;
	//private members
private:
	//public members
public:
	int orbitingBodyIndex = -1;
	float radius;
	float orbitalSpeed;
	float orbitDistance;
	float orbitAmount = 0.0f;
	//private functions
private:

	//public functions
public:
	void init(char* modelFile, glm::vec3 initialPosition, glm::vec3 initialRotation);
	void SetPosition(glm::vec3 position);
	void SetOrbit(int orbitingBodyIndex, float speed, float distance);
	void UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime);
	std::pair<float, float> GetMinMaxZoom();
};