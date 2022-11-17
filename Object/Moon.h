#pragma once
#include "Object.h"
class Moon : public Object
{
private:
	float orbitAmount = 0.0f;
	glm::vec3 orbitCentre;
public:
	float orbitalSpeed;
	float orbitDistance;
private:
public:
	void init(char* modelFile, float speed, float distance);
	void SetTransform(glm::vec3 position, glm::vec3 rotation);
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider);
};

