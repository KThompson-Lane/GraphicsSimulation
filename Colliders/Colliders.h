#pragma once
#include "../GL/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"


//Abstract base collider class
class Collider {
public:
	virtual void CreateGeometry(CShader& shader) = 0;
	virtual void DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat) = 0;
	virtual bool InCollision(Collider* otherCol, float distance) = 0;
	virtual float CalculatePenetration(Collider* otherCol, float distance) = 0;
protected: 
	unsigned int m_vaoID;		    // vertex array object
	unsigned int m_vboID;			// VBO for vertex data
	GLuint ibo;                     //identifier for the triangle indices

	int numOfVerts;
	int numOfTris;

	float* verts;
	unsigned int* tInds;
};

//Sphere collider class
class SphereCollider : public Collider {
private:
	glm::vec3 centre;	//centre of the collider
	float radius;		//radius of the sphere
private:
	bool InCollision(SphereCollider* otherCol, float distance);
	float CalculatePenetration(SphereCollider* otherCol, float distance);
public:
	SphereCollider();
	SphereCollider(float r, glm::vec3 centre);
	void CreateGeometry(CShader& shader);
	void DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat);
	bool InCollision(Collider* otherCol, float distance);
	float CalculatePenetration(Collider* otherCol, float distance);
	float GetRadius() const { return radius; };
};

class BoxCollider : public Collider {
private:
	glm::vec3 centre, xAxis, yAxis, zAxis, halfSize; //Vectors OBB
private:
	bool InCollision(SphereCollider* otherCol, float distance);
	float CalculatePenetration(SphereCollider* otherCol, float distance);
public:
	BoxCollider();
	BoxCollider(glm::vec3 halfSize, glm::vec3 centre);
	void CreateGeometry(CShader& shader);
	void DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat);
	bool InCollision(Collider* otherCol, float distance);
	float CalculatePenetration(Collider* otherCol, float distance);
};