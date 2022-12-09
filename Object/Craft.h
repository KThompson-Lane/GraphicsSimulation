#pragma once
#include "CelestialBody.h"
class Craft : public CelestialBody
{
private:
	glm::vec3 velocity;
	glm::vec3 landingPosition;
	Object* landedBody = nullptr;
public:
	bool inOrbit = true;
	bool landed = false;
	Craft() : CelestialBody("satellite") {}
	void CollideWithPlayer(glm::vec3 impactVelocity);
	void UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime);
	void AddForce(glm::vec3 force);
	bool CheckCollision(Object& other);
};