#include "Object.h"
/*
	//Material properties
	float Material_Ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
	float Material_Diffuse[4] = {0.8f, 0.8f, 0.5f, 1.0f};
	float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
	float Material_Shininess = 50;

	//Light Properties
	float Light_Ambient_And_Diffuse[4] = {0.8f, 0.8f, 0.6f, 1.0f};
	float Light_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
	float LightPos[4] = {0.0f, 0.0f, 1.0f, 0.0f};
*/
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
		//model.CalcCentrePoint();
		//model.CentreOnZero();

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

void Object::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix)
{
	//Displacement stuffs
	/*
	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "displacement"), amount);
	*/

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(objectShader.GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//Set the view matrix in the shader
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	//lighting stuffs
	/*
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "material_shininess"), Material_Shininess);
	*/

	//Set the modelview matrix in the shader

	//objectPosition[3][0]
	//glm::mat4 modelmatrix = glm::translate(objectPosition, glm::vec3(0.0,0.0,0.0));
	ModelViewMatrix = viewingMatrix * objectModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//Set the normal matrix in the shader
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(objectShader.GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);


	//Do a render
	model.DrawElementsUsingVBO(&objectShader);

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

	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader
	*/
}

void Object::UpdateTransform(glm::vec3 incTranslation, glm::vec3 incRotation)
{
	//First rotate object, then translate it.
	objectModelMatrix = glm::rotate(objectModelMatrix, incRotation.x, glm::vec3(1, 0, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, incRotation.y, glm::vec3(0, 1, 0));
	objectModelMatrix = glm::rotate(objectModelMatrix, incRotation.z, glm::vec3(0, 0, 1));
	objectModelMatrix = glm::translate(objectModelMatrix, incTranslation);
}

glm::vec3 Object::GetObjectWorldPosition()
{
	//object model matrix is [column][row] notation e.g. Tx Ty Tz = [3][0] [3][1] [3][2]
	return glm::vec3(objectModelMatrix[3][0], objectModelMatrix[3][1], objectModelMatrix[3][2]);
}
