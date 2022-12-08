#pragma once
#include "../GL/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"
#include "../Transform/Transform.h"

//Abstract base collider class
class Collider {
public:
	Collider(Transform* t) : transform(t){}
	virtual void CreateGeometry(CShader& shader) = 0;
	virtual void DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat) = 0;
	virtual bool InCollision(Collider* otherCol) = 0;
	virtual glm::vec3 CalculatePenetration(Collider* otherCol) = 0;
protected: 
	unsigned int m_vaoID;		    // vertex array object
	unsigned int m_vboID;			// VBO for vertex data
	GLuint ibo;                     //identifier for the triangle indices
	int numOfVerts;
	int numOfTris;
	float* verts;
	unsigned int* tInds;
public:
	Transform* transform;
};

//Sphere collider class
class SphereCollider : public Collider {
private:
	float radius;		//radius of the sphere
private:
	bool InCollision(SphereCollider* otherCol);
	glm::vec3 CalculatePenetration(SphereCollider* otherCol);
public:
	SphereCollider(float r, Transform* t) : radius(r), Collider(t) { }
	void CreateGeometry(CShader& shader);
	void DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat);
	bool InCollision(Collider* otherCol);
	glm::vec3 CalculatePenetration(Collider* otherCol);
	float GetRadius() const { return radius; };
};

class BoxCollider : public Collider {
private:
	glm::vec3 halfSize; //Vectors OBB
private:
	bool InCollision(SphereCollider* otherCol);
	glm::vec3 CalculatePenetration(SphereCollider* otherCol);
public:
	BoxCollider(glm::vec3 halfSize, Transform* transform) : halfSize(halfSize), Collider(transform) { }
	void CreateGeometry(CShader& shader);
	void DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat);
	bool InCollision(Collider* otherCol);
	glm::vec3 CalculatePenetration(Collider* otherCol);
};