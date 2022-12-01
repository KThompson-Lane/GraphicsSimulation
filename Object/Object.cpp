#include "Object.h"

struct Material{
	//kA
	glm::vec3 ambient;
	//kD
	glm::vec3 diffuse;
	//kS
	glm::vec3 specular;
	//nI
	float shininess;
};
Material material;

//Displacement stuff
float amount = 0.1f;
float temp = 0.0f;

void Object::init(char* modelFile)
{
	std::cout << " loading model " << std::endl;
	if (objectLoader.LoadModel(modelFile))//returns true if the model is loaded
	{
		std::cout << " model loaded " << std::endl;

		//copy data from the OBJLoader object to the threedmodel class
		model.ConstructModelFromOBJLoader(objectLoader);

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		model.CalcCentrePoint();
		model.CentreOnZero();

		//Create bounding sphere:
		boundingShader = CShader();
		if (!boundingShader.CreateShaderProgram("SimpleShader", "glslfiles/basic.vert", "glslfiles/basic.frag"))
		{
			std::cout << "failed to load shader" << std::endl;
		}
		boundingSphere.setCentre(0, 0, 0);
		boundingSphere.setRadius(model.CalcBoundingSphere());
		boundingSphere.constructGeometry(&boundingShader, 16);
		model.InitVBO(&objectShader);
	}
	else
	{
		std::cout << " model failed to load " << std::endl;
	}

	//Set object model matrix and rotation to identity
	objectModelMatrix = glm::mat4(1.0);
	objectRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
}

void Object::setupShader(char* shaderName, char* vertPath, char* fragPath)
{
	objectShader = CShader();
	if (!objectShader.CreateShaderProgram(shaderName, vertPath, fragPath))
	{
		std::cout << "failed to load shader" << std::endl;
	}

	//REMOVE THIS AND INSTEAD SET IN OBJ LOAD
	material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	material.shininess = 50.0f;

	glUseProgram(objectShader.GetProgramObjID());  // use the shader
	glEnable(GL_TEXTURE_2D);
}

void Object::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights)
{
	glDisable(GL_CULL_FACE);

	glUseProgram(objectShader.GetProgramObjID());  // use the shader
	//Displacement stuffs
	
	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "displacement"), amount);
	

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(objectShader.GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//Set the view matrix in the shader
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	//TEST CODE ONLY USE ONE LIGHT FOR NOW
	//TRANSFORM POSITION INTO EYESPACE
	glm::vec3 lightEyePos = glm::vec3(viewingMatrix * glm::vec4(lights[0].position, 1));
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].position"), lightEyePos.x, lightEyePos.y, lightEyePos.z);

	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].ambient"), lights[0].ambient.x, lights[0].ambient.y, lights[0].ambient.z);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].diffuse"), lights[0].diffuse.x, lights[0].diffuse.y, lights[0].diffuse.z);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].specular"), lights[0].specular.x, lights[0].specular.y, lights[0].specular.z);

	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].constant"), lights[0].constant);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].linear"), lights[0].linear);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "pointLights[0].quadratic"), lights[0].quadratic);

	//Set the modelview matrix in the shader
	objectModelMatrix = glm::translate(glm::mat4(1.0), objectPosition);
	objectModelMatrix *= glm::toMat4(objectRotation);

	ModelViewMatrix = viewingMatrix * objectModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//Set the normal matrix in the shader
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(objectShader.GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);


	//Do a render
	model.DrawElementsUsingVBO(&objectShader);

	if (showCollider)
	{
		//If we want, render bounding spere
		glDisable(GL_CULL_FACE);
		//glUseProgram(boundShader.GetProgramObjID());  // use the shader
		glUseProgram(boundingShader.GetProgramObjID());
		GLuint projMatLocation = glGetUniformLocation(boundingShader.GetProgramObjID(), "ProjectionMatrix");
		glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(boundingShader.GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		boundingSphere.render();
		//OpenGL Stuff
		glEnable(GL_CULL_FACE);
	}

	//bounding box stuffs
	/*
	//Switch to basic shader to draw the lines for the bounding boxes
	glUseProgram(myBasicShader->GetProgramObjID());
	projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	
	//model.DrawAllBoxesForOctreeNodes(myBasicShader);
	//model.DrawBoundingBox(myBasicShader);
	//model.DrawOctreeLeaves(myBasicShader);
	*/

	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(objectShader.GetProgramObjID());  // use the shader
}

void Object::Move(glm::vec3 direction, float amount)
{
	objectPosition += (direction * amount);
}

void Object::Rotate(float pitchIn, float yawIn, float rollIn)
{
	glm::quat pitch = glm::angleAxis(glm::radians(pitchIn), Side());
	glm::quat yaw = glm::angleAxis(glm::radians(yawIn), Up());
	glm::quat roll = glm::angleAxis(glm::radians(rollIn), Forward());
	objectRotation = (pitch * yaw * roll * objectRotation);
}

glm::vec3 Object::GetObjectWorldPosition()
{
	return objectPosition;
}
glm::vec3 Object::Side()
{
	//Retrieves the Side direction
	return glm::normalize(glm::vec3(objectModelMatrix[0][0], objectModelMatrix[0][1], objectModelMatrix[0][2]));
}

glm::vec3 Object::Up()
{
	//Retrieves the up direction
	return glm::normalize(glm::vec3(objectModelMatrix[1][0], objectModelMatrix[1][1], objectModelMatrix[1][2]));
}

glm::vec3 Object::Forward()
{
	//Retrieves the Forward direction
	return glm::normalize(glm::vec3(objectModelMatrix[2][0], objectModelMatrix[2][1], objectModelMatrix[2][2]));
}

float Object::GetColliderSphereRadius()
{
	return boundingSphere.GetRadius();
}
