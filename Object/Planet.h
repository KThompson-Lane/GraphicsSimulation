#pragma once
#include "Object.h";
class Planet : public Object
{
	//private members
private:
	Planet* orbitingBody = nullptr;
	float orbitalSpeed;
	float orbitDistance;
	float orbitAmount = 0.0f;

	//public members
public:
	
	//private functions
private:

	//public functions
public:
	void init(char* modelFile, glm::vec3 initialPosition, glm::vec3 initialRotation);
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider);
	void SetOrbit(Planet* orbitingBody, float speed, float distance);
};