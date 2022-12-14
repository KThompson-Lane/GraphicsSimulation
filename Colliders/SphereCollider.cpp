#include "Colliders.h"

#define level 16
#define PI 3.14159265358979323846f

void SphereCollider::CreateGeometry(CShader& shader)
{
	
	glm::vec3 centre = glm::vec3(0.0, 0.0, 0.0);
	//create the memory for the sphere
	verts = new float[((level - 2) * level + 2) * 3];
	tInds = new unsigned int[(((level - 3) * (level - 1) + (level - 1)) * 2) * 3];

	numOfTris = (((level - 3) * (level - 1) + (level - 1)) * 2);
	numOfVerts = ((level - 2) * level + 2);

	//populate the arrays
	float theta, phi;
	int vertCount = 0;
	int i, j, t;

	for (t = 0, j = 1; j < level - 1; j++)
	{
		for (i = 0; i < level; i++)
		{
			theta = float(j) / (level - 1) * PI;
			phi = float(i) / (level - 1) * PI * 2;

			verts[vertCount + t] = ((sinf(theta) * cosf(phi)) * radius) + centre.x;
			verts[vertCount + t + 1] = (cosf(theta) * radius) + centre.y;
			verts[vertCount + t + 2] = ((-sinf(theta) * sinf(phi)) * radius) + centre.z;
			t += 3;
		}
	}
	verts[vertCount + t] = centre.x;
	verts[vertCount + t + 1] = radius + centre.y;
	verts[vertCount + t + 2] = centre.z;
	t += 3;

	verts[vertCount + t] = centre.x;
	verts[vertCount + t + 1] = -radius + centre.y;
	verts[vertCount + t + 2] = centre.z;
	t += 3;

	int vc3 = vertCount / 3;
	int triCount = 0;
	for (t = 0, j = 0; j < level - 3; j++)
	{
		for (i = 0; i < level - 1; i++)
		{
			tInds[triCount + t] = vc3 + ((j)*level + i);    t++;
			tInds[triCount + t] = vc3 + ((j + 1) * level + i + 1);    t++;
			tInds[triCount + t] = vc3 + ((j)*level + i + 1);    t++;
			tInds[triCount + t] = vc3 + ((j)*level + i);    t++;
			tInds[triCount + t] = vc3 + ((j + 1) * level + i);    t++;
			tInds[triCount + t] = vc3 + ((j + 1) * level + i + 1);  t++;
		}
	}
	for (i = 0; i < level - 1; i++)
	{
		tInds[triCount + t] = vc3 + ((level - 2) * level);  t++;
		tInds[triCount + t] = vc3 + (i);    t++;
		tInds[triCount + t] = vc3 + (i + 1);    t++;
		tInds[triCount + t] = vc3 + ((level - 2) * level + 1);    t++;
		tInds[triCount + t] = vc3 + ((level - 3) * level + i + 1);    t++;
		tInds[triCount + t] = vc3 + ((level - 3) * level + i);  t++;
	}
	triCount += t;

	//assign the data to the GPU
	// VAO allocation
	glGenVertexArrays(1, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	//initialises data storage of vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

	GLint vertexLocation = glGetAttribLocation(shader.GetProgramObjID(), "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfTris * 3 * sizeof(unsigned int), tInds, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SphereCollider::DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat)
{
	glDisable(GL_CULL_FACE);

	//Use the bounding shader
	glUseProgram(shader.GetProgramObjID());
	GLuint projMatLocation = glGetUniformLocation(shader.GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &projMat[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &mvm[0][0]);
	
	//draw objects
	glBindVertexArray(m_vaoID);		// select VAO
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, numOfTris * 3, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Done	

	glBindVertexArray(0); //unbind the vertex array object
	glEnable(GL_CULL_FACE);

}

bool SphereCollider::InCollision(SphereCollider* othercol)
{
	float colliderRadi = othercol->GetRadius() + radius;
	float dist = distance(othercol->transform->position, transform->position);
	return dist <= colliderRadi;
}

bool SphereCollider::InCollision(Collider* othercol)
{
	SphereCollider* otherCollider = dynamic_cast<SphereCollider*>(othercol);
	if (otherCollider != NULL)
	{
		return this->InCollision(otherCollider);
	}
}

glm::vec3 SphereCollider::CalculatePenetration(SphereCollider* othercol)
{
	//First inverse transform other collider position into local space
	glm::vec3 localSphereCentre = glm::inverse(transform->ModelMatrix) * glm::vec4(othercol->transform->position, 1.0);

	//Our centre is 0,0,0
	float distance = glm::length(localSphereCentre);
	float radiusSum = this->radius + othercol->GetRadius();
	glm::vec3 translation = glm::normalize(-localSphereCentre) * (radiusSum - distance);
	glm::vec3 WorldTranslation = transform->ModelMatrix * glm::vec4(translation, 1.0);
	return WorldTranslation;
}

glm::vec3 SphereCollider::CalculatePenetration(Collider* othercol)
{
	SphereCollider* otherCollider = dynamic_cast<SphereCollider*>(othercol);
	if (otherCollider != NULL)
	{
		return this->CalculatePenetration(otherCollider);
	}
}
