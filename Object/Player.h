#pragma once
#include "Object.h"
class Player : public Object
{
	//private members
private:

	//public members
public:

	//private functions
private:

	//public functions
public:
	void UpdateTransform(glm::vec3 incTranslation, glm::vec3 incRotation);
	glm::mat4 CameraMount();
	glm::mat4 CameraTarget();
};

