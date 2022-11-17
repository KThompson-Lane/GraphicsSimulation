#include "Object.h"

///Material properties (From the .mtl file) 
// kA 
float Material_Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
// kD
float Material_Diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
// kS
float Material_Specular[4] = {0.5f,0.5f,0.5f,1.0f};
// nI (reflection)
float Material_Shininess = 50;

//Displacement stuff
float amount = 0.1f;
float temp = 0.0f;

//TODO: Move this into a separate class for "lights"
//Light Properties
float Light_Ambient_And_Diffuse[4] = {0.8f, 0.8f, 0.6f, 1.0f};
float Light_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
float LightPos[4] = {0.0f, 0.0f, 1.0f, 0.0f};

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
		//TEST BOUNDING SPHERE
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

	//Set object position to identityMatrix
	objectModelMatrix = glm::mat4(1.0);
}

void Object::setupShader(char* shaderName, char* vertPath, char* fragPath)
{
	objectShader = CShader();
	if (!objectShader.CreateShaderProgram(shaderName, vertPath, fragPath))
	{
		std::cout << "failed to load shader" << std::endl;
	}
	glUseProgram(objectShader.GetProgramObjID());  // use the shader
	glEnable(GL_TEXTURE_2D);
}

void Object::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider)
{
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

	//lighting stuffs
	
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "material_shininess"), Material_Shininess);
	

	//Set the modelview matrix in the shader

	objectModelMatrix = glm::translate(glm::mat4(1.0), objectPosition);
	objectModelMatrix = glm::rotate(objectModelMatrix, glm::radians(objectRotation.x), glm::vec3(1, 0, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, glm::radians(objectRotation.y), glm::vec3(0, 1, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, glm::radians(objectRotation.z), glm::vec3(0, 0, 1));
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

glm::vec3 Object::GetObjectWorldPosition()
{
	//object model matrix is [column][row] notation e.g. Tx Ty Tz = [3][0] [3][1] [3][2]
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


