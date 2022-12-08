#include "Colliders.h"

void BoxCollider::CreateGeometry(CShader& shader)
{
	//create the memory for the sphere
	verts = new float[8*3];
	tInds = new unsigned int[12 * 3];

	numOfTris = (6 * 2);
	numOfVerts = (8);

	//populate the arrays
	verts[0] = -halfSize.x;   verts[1] = -halfSize.y;	verts[2] = -halfSize.z;
	verts[3] = -halfSize.x;   verts[4] = halfSize.y;	verts[5] = -halfSize.z;
	verts[6] = halfSize.x;	  verts[7] = halfSize.y;	verts[8] = -halfSize.z;
	verts[9] = halfSize.x;    verts[10] = -halfSize.y;	verts[11] = -halfSize.z;

	verts[12] = -halfSize.x;  verts[13] = -halfSize.y;  verts[14] = halfSize.z;
	verts[15] = -halfSize.x;  verts[16] = halfSize.y;   verts[17] = halfSize.z;
	verts[18] = halfSize.x;   verts[19] = halfSize.y;	verts[20] = halfSize.z;
	verts[21] = halfSize.x;   verts[22] = -halfSize.y;  verts[23] = halfSize.z;
	
	tInds[0] = 0; tInds[1] = 1;  tInds[2] = 2;
	tInds[3] = 0; tInds[4] = 2;  tInds[5] = 3;
	tInds[6] = 4; tInds[7] = 6;  tInds[8] = 5;
	tInds[9] = 4; tInds[10] = 7; tInds[11] = 6;
	tInds[12] = 1; tInds[13] = 5; tInds[14] = 6;
	tInds[15] = 1; tInds[16] = 6; tInds[17] = 2;
	tInds[18] = 0; tInds[19] = 7; tInds[20] = 4;
	tInds[21] = 0; tInds[22] = 3; tInds[23] = 7;
	tInds[24] = 0; tInds[25] = 5; tInds[26] = 1;
	tInds[27] = 0; tInds[28] = 4; tInds[29] = 5;
	tInds[30] = 3; tInds[31] = 2; tInds[32] = 7;
	tInds[33] = 2; tInds[34] = 6; tInds[35] = 7;

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

void BoxCollider::DrawCollider(CShader& shader, glm::mat4& mvm, glm::mat4& projMat)
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

bool BoxCollider::InCollision(SphereCollider* otherCol)
{
	//First inverse transform other collider position into local space
	glm::vec3 localSphereCentre = glm::inverse(transform->ModelMatrix) * glm::vec4(otherCol->transform->position, 1.0);
	
	//Test inverse transform box position:
	glm::vec3 localCentre = glm::inverse(transform->ModelMatrix) * glm::vec4(transform->position, 1.0);

	//glm::vec3 clamped = glm::clamp(localSphereCentre, -halfSize, halfSize);
	//Testing individually clamped values:
	float closeX = std::max(-halfSize.x, std::min(localSphereCentre.x, halfSize.x));
	float closeY = std::max(-halfSize.y, std::min(localSphereCentre.y, halfSize.y));
	float closeZ = std::max(-halfSize.z, std::min(localSphereCentre.z, halfSize.z));

	glm::vec3 closest = glm::vec3(closeX, closeY, closeZ);

	glm::vec3 difference = localSphereCentre - closest;
	return glm::length(difference) < otherCol->GetRadius();
}

bool BoxCollider::InCollision(Collider* otherCol) 
{
	SphereCollider* otherCollider = dynamic_cast<SphereCollider*>(otherCol);
	if (otherCollider != NULL)
	{
		return this->InCollision(otherCollider);
	}
}

glm::vec3 BoxCollider::CalculatePenetration(SphereCollider* otherCol)
{
	//First inverse transform other collider position into local space
	glm::vec3 localSphereCentre = glm::inverse(transform->ModelMatrix) * glm::vec4(otherCol->transform->position, 1.0);


	//Test inverse transform box position:
	glm::vec3 localCentre = glm::inverse(transform->ModelMatrix) * glm::vec4(transform->position, 1.0);

	//glm::vec3 clamped = glm::clamp(localSphereCentre, -halfSize, halfSize);
	//Testing individually clamped values:
	float closeX = std::max(-halfSize.x, std::min(localSphereCentre.x, halfSize.x));
	float closeY = std::max(-halfSize.y, std::min(localSphereCentre.y, halfSize.y));
	float closeZ = std::max(-halfSize.z, std::min(localSphereCentre.z, halfSize.z));

	glm::vec3 closest = glm::vec3(closeX, closeY, closeZ);

	glm::vec3 difference = localSphereCentre - closest;
	float length = glm::length(difference);
	float amount = otherCol->GetRadius() - glm::length(difference);
	glm::vec3 direction = glm::normalize(difference);
	glm::vec3 translation = (otherCol->GetRadius() - glm::length(difference)) * glm::normalize(difference);
	glm::vec3 test = transform->ModelMatrix * glm::vec4(translation, 1.0);
	return test;
}

glm::vec3 BoxCollider::CalculatePenetration(Collider* otherCol)
{
	SphereCollider* otherCollider = dynamic_cast<SphereCollider*>(otherCol);
	if (otherCollider != NULL)
	{
		return this->CalculatePenetration(otherCollider);
	}
}