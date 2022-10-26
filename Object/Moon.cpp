#include "Moon.h"

void Moon::init(char* modelFile, float speed, float distance)
{
	orbitalSpeed = speed;
	orbitDistance = distance;
	Object::init(modelFile);
}
void Moon::SetTransform(glm::vec3 position, glm::vec3 rotation)
{
	objectModelMatrix = glm::rotate(objectModelMatrix, rotation.x, glm::vec3(1, 0, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, rotation.y, glm::vec3(0, 1, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, rotation.z, glm::vec3(0, 0, 1));
	objectModelMatrix = glm::translate(objectModelMatrix, position);
	objectModelMatrix = glm::translate(objectModelMatrix, glm::vec3(orbitDistance, 0.0f, 0.0f));
}
void Moon::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix)
{
	objectModelMatrix = glm::translate(objectModelMatrix, glm::vec3(-orbitDistance, 0.0f, 0.0f));
	objectModelMatrix = glm::rotate(objectModelMatrix, orbitalSpeed/100, glm::vec3(0.0f, 1.0f, 0.0f));
	objectModelMatrix = glm::translate(objectModelMatrix, glm::vec3(orbitDistance, 0.0f, 0.0f));
	Object::render(viewingMatrix, ProjectionMatrix);
}

