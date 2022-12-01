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
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight);
	float GetSpeed();
	float GetRotationSpeed();
};

