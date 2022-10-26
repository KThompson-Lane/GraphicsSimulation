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
	objectModelMatrix = glm::rotate(objectModelMatrix, rotation.x, glm::vec3(1, 0, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, rotation.y, glm::vec3(0, 1, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, rotation.z, glm::vec3(0, 0, 1));
	objectModelMatrix = glm::translate(objectModelMatrix, position);
	this->moon->SetTransform(position, rotation);
}

void Planet::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix) 
{
	if (moon != nullptr)
	{
		//we have a moon
		moon->render(viewingMatrix, ProjectionMatrix);
	}
	Object::render(viewingMatrix, ProjectionMatrix);
}