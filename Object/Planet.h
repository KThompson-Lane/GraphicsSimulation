#pragma once
#include "Object.h";
#include "Moon.h";
class Planet : public Object
{
	//private members
private:
	Moon* moon = nullptr;
	//public members
public:
	
	//private functions
private:

	//public functions
public:
	void init(char* modelFile, Moon* moon);
	void init(char* modelFile, Moon* moon, glm::vec3 initialPosition, glm::vec3 initialRotation);
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider);

};