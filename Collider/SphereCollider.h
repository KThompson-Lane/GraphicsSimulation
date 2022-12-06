#pragma once
#include "Collider.h"

class SphereCollider : public Collider {
private:
	glm::vec3 centre;	//centre of the sphere
	float radius;		//radius of the sphere
public:
	SphereCollider();
	SphereCollider(float r, glm::vec3 centre);
	void CreateGeometry(CShader& shader);
	void DrawCollider(CShader& shader);
	void InCollision(Collider& otherCol);
	float GetRadius() const { return radius; };
};