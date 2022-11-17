#include "Moon.h"

void Moon::init(char* modelFile, float speed, float distance)
{
	orbitalSpeed = speed;
	orbitDistance = distance;
	Object::init(modelFile);
}
void Moon::SetTransform(glm::vec3 orbitPosition, glm::vec3 rotation)
{
	objectPosition = orbitPosition;
	orbitCentre = orbitPosition;
	objectRotation = rotation;
}
void Moon::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix , bool showCollider)
{
	orbitAmount += orbitalSpeed*2;
	if (orbitAmount >= 360.0f)
	{
		orbitAmount -= 360.0f;
	}
	else if (orbitAmount < 0.0f)
	{
		orbitAmount += 360.0f;
	}
	

	//Try to do this without matrix operations if possible.

	//Create new matrix for orbiting
	glm::mat4 orbitMatrix = glm::translate(glm::mat4(1.0), orbitCentre);
		
	orbitMatrix = glm::rotate(orbitMatrix, glm::radians(orbitAmount), glm::vec3(0.0, 1.0, 0.0));
	orbitMatrix = glm::translate(orbitMatrix, glm::vec3(orbitDistance, 0.0, 0.0));
	//Rotate by X then rotate by Y 
	objectPosition.x = orbitMatrix[3][0];
	objectPosition.y = orbitMatrix[3][1];
	objectPosition.z = orbitMatrix[3][2];

	Object::render(viewingMatrix, ProjectionMatrix, showCollider);
}

