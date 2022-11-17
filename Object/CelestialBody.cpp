#include "CelestialBody.h"

void CelestialBody::init(char* modelFile, glm::vec3 position, glm::vec3 rotation)
{
	Object::init(modelFile);
	objectPosition = position;
	objectRotation = glm::quat(glm::radians(rotation));
}

void CelestialBody::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider)
{
	if (orbitingBody != nullptr)
	{
		orbitAmount += orbitalSpeed;
		if (orbitAmount >= 360.0f)
		{
			orbitAmount -= 360.0f;
		}
		else if (orbitAmount < 0.0f)
		{
			orbitAmount += 360.0f;
		}

		//Try to do this without matrix operations if possible. Quaternions?

		//Create new matrix for orbiting at the position of the body we are orbiting
		glm::mat4 orbitMatrix = glm::translate(glm::mat4(1.0), orbitingBody->GetObjectWorldPosition());

		orbitMatrix = glm::rotate(orbitMatrix, glm::radians(orbitAmount), glm::vec3(0.0, 1.0, 0.0));
		orbitMatrix = glm::translate(orbitMatrix, glm::vec3(orbitDistance, 0.0, 0.0));
		//Rotate by X then rotate by Y 
		objectPosition.x = orbitMatrix[3][0];
		objectPosition.y = orbitMatrix[3][1];
		objectPosition.z = orbitMatrix[3][2];
	}
	Object::render(viewingMatrix, ProjectionMatrix, showCollider);
}

void CelestialBody::SetOrbit(CelestialBody* body, float speed, float distance)
{
	orbitingBody = body;
	orbitalSpeed = speed;
	orbitDistance = distance;
}