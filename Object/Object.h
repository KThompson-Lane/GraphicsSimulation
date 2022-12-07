#pragma once
#include <iostream>

#include "../gl/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_inverse.hpp"
#include "../3dStruct/threeDModel.h"
#include "../Colliders/Colliders.h"

#include "../Transform/Transform.h"

//Lighting
#include "../Light/Light.h"

class Object
{
//private members
private:
	CShader objectShader;
	char* modelFile;
	COBJLoader objectLoader;
	glm::mat4 ModelViewMatrix = glm::mat4(1.0);  // matrix for the modelling and viewing
//protected members
protected:
	CShader boundingShader;
	float amount = 0.0f;
//public members
public:
	CThreeDModel model;
	Transform* transform = nullptr;
	Collider* collider = nullptr;
	const std::string tag;
//private functions
private:

//public functions
public:
	Object(std::string tag) : tag(tag) { }
	void init(char* modelFile);
	void setupShader(char*, char*, char*);
	void AddSphereCollider();
	void AddBoxCollider();
	void render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight);
	float GetColliderSphereRadius();
	bool CheckCollision(Object& other);
	virtual std::pair<float, float> GetMinMaxZoom() = 0;
};

