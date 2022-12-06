#pragma once
#include <iostream>

#include "../gl/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_inverse.hpp"
#include "../3dStruct/threeDModel.h"
#include "../Colliders/Colliders.h"

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
	char* modelFile;
	COBJLoader objectLoader;
	glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing
//protected members
protected:
	CShader boundingShader;
	glm::mat4 objectModelMatrix;
	glm::vec3 objectPosition;
	glm::quat objectRotation;
	float amount = 0.0f;
//public members
public:
	CThreeDModel model;
	Collider* collider = nullptr;
//private functions
private:

//public functions
public:
	void init(char* modelFile);
	void setupShader(char*, char*, char*);
	void AddSphereCollider();
	void AddBoxCollider();

	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight);
	void Move(glm::vec3 direction, float amount);
	void Rotate(float pitchIn, float yawIn, float rollIn);
	glm::vec3 Side();
	glm::vec3 Up();
	glm::vec3 Forward();
	float GetColliderSphereRadius();
	bool CheckCollision(Object& other);
	glm::vec3 GetObjectWorldPosition() const { return objectPosition; }
	glm::quat GetObjectRotation() const { return objectRotation; }
};

