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

void Object::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight)
{
	glUseProgram(objectShader.GetProgramObjID());  // use the shader

	//Displacement stuffs
	
	//Part for displacement shader.
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "displacement"), amount);
	

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(objectShader.GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//Set the view matrix in the shader
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glm::vec3 lightEyePos;
	for (int i = 0; i < lights.size(); i++)
	{
		//TRANSFORM POSITION INTO EYESPACE
		lightEyePos = viewingMatrix * glm::vec4(lights[i].position, 1);

		//Construct light string and set values in shader
		std::string lightArrayString = "pointLights[";
		lightArrayString += std::to_string(i);
		lightArrayString += "]";
		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString+".position").c_str()), lightEyePos.x, lightEyePos.y, lightEyePos.z);

		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".ambient").c_str()), lights[i].ambient.x, lights[i].ambient.y, lights[i].ambient.z);
		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".diffuse").c_str()), lights[i].diffuse.x, lights[i].diffuse.y, lights[i].diffuse.z);
		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".specular").c_str()), lights[i].specular.x, lights[i].specular.y, lights[i].specular.z);

		glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".constant").c_str()), lights[i].constant);
		glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".linear").c_str()), lights[i].linear);
		glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".quadratic").c_str()), lights[i].quadratic);
	}

	lightEyePos = viewingMatrix * glm::vec4(playerSpotLight.position, 1);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.position"), lightEyePos.x, lightEyePos.y, lightEyePos.z);

	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.ambient"), playerSpotLight.ambient.x, playerSpotLight.ambient.y, playerSpotLight.ambient.z);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.diffuse"), playerSpotLight.diffuse.x, playerSpotLight.diffuse.y, playerSpotLight.diffuse.z);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.specular"), playerSpotLight.specular.x, playerSpotLight.specular.y, playerSpotLight.specular.z);

	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.constant"), playerSpotLight.constant);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.linear"), playerSpotLight.linear);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.quadratic"), playerSpotLight.quadratic);

	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.direction"), playerSpotLight.direction.x, playerSpotLight.direction.y, playerSpotLight.direction.z);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.cutOff"), glm::cos(glm::radians(playerSpotLight.cutOff)));

	glUniform1i(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.enabled"), playerSpotLight.active);
	
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
