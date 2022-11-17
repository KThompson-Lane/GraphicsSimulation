#pragma once
#include "Object.h";
class CelestialBody : public Object
{
	//private members
private:
	CelestialBody* orbitingBody = nullptr;
	float orbitalSpeed;
	float orbitDistance;
	float orbitAmount = 0.0f;
	float gravitationalPull;
	//public members
public:
	//private functions
private:

	//public functions
public:
	void init(char* modelFile, glm::vec3 initialPosition, glm::vec3 initialRotation, float gravity);
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider);
	void SetOrbit(CelestialBody* orbitingBody, float speed, float distance);
	float GetGravityDistance();
};