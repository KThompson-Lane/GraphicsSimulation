#pragma once
#include "CelestialBody.h"
class Craft : public CelestialBody
{
private:
	glm::vec3 velocity;
public:
	bool inOrbit = true;
	Craft() : CelestialBody("satellite") {}
	void CollideWithPlayer(glm::vec3 impactVelocity);
	void UpdateOrbit(glm::vec3 orbitBodyPosition, float deltaTime);
};