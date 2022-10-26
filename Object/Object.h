#pragma once
#include <iostream>

#include "../gl/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_inverse.hpp"
#include "../3dStruct/threeDModel.h"
class Object
{
//private members
private:
	CShader objectShader;
	char* modelFile;
	COBJLoader objectLoader;
	glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

protected:
	glm::mat4 objectModelMatrix;
	glm::vec3 objectPosition;
	glm::vec3 objectRotation;
//public members
public:
	CThreeDModel model;
//private functions
private:

//public functions
public:
	//Object(char*);
	void init(char* modelFile);
	void setupShader(char*, char*, char*);
	//TODO: Change this to take in a collection of Light objects and iterate over them when rendering.
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix);
	glm::vec3 GetObjectWorldPosition();
	glm::vec3 Up();
	glm::vec3 Right();
	glm::vec3 Forward();

};

