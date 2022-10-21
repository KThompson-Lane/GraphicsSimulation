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
	glm::mat4 objectRotation;
	glm::mat4 objectPosition;

//public members
public:
	CThreeDModel model;

//private functions
private:

//public functions
public:
	//Object(char*);
	void init(char*);
	void setupShader(char*, char*, char*);
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix);
	void UpdateTransform(glm::vec3 position, glm::vec3 rotation);
};

