#include "Planet.h"

void Planet::init(char* modelFile, Moon* moon)
{
	this->moon = moon;
	Object::init(modelFile);
}

void Planet::init(char* modelFile, Moon* moon, glm::vec3 position, glm::vec3 rotation)
{
	this->moon = moon;
	Object::init(modelFile);
	objectPosition = position;
	objectRotation = rotation;
	this->moon->SetTransform(position, rotation);
}

void Planet::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider)
{
	if (moon != nullptr)
	{
		//we have a moon
		moon->render(viewingMatrix, ProjectionMatrix, showCollider);
	}
	Object::render(viewingMatrix, ProjectionMatrix, showCollider);
}