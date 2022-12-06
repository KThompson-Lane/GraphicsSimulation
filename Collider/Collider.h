#pragma once
#include "../GL/glew.h"
#include "../shaders/Shader.h"
#include "../glm/glm.hpp"

class Collider {
public:
	virtual void CreateGeometry(CShader& shader) = 0;
	virtual void DrawCollider(CShader& shader) = 0;
	//virtual bool inCollision(Collider& otherCol) = 0;
protected: 
	unsigned int m_vaoID;		    // vertex array object
	unsigned int m_vboID;			// VBO for vertex data
	GLuint ibo;                     //identifier for the triangle indices

	int numOfVerts;
	int numOfTris;

	float* verts;
	unsigned int* tInds;
};