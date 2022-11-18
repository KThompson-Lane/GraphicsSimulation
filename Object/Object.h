#pragma once
#include <iostream>

#include "../gl/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_inverse.hpp"
#include "../3dStruct/threeDModel.h"
#include "../sphere/Sphere.h"

//Angry 4thDimensional spinning noises
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"

//Lighting
#include "../Light/Light.h"

class Object
{
//private members
private:
	CShader objectShader;
	CShader boundingShader;
	char* modelFile;
	COBJLoader objectLoader;
	glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing
	Sphere boundingSphere;

protected:
	glm::mat4 objectModelMatrix;
	glm::vec3 objectPosition;
	glm::quat objectRotation;
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
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, Light& light);
	void Move(glm::vec3 direction, float amount);
	glm::vec3 GetObjectWorldPosition();
	glm::vec3 Side();
	glm::vec3 Up();
	glm::vec3 Forward();
	float GetColliderSphereRadius();
};

